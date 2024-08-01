#include "Client.hpp"
#include "CgiHandler.hpp"
#include "Configuration.hpp"
#include "Socket.hpp"
#include "http_utils.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

Client::Client(Socket *socket, size_t id, Configuration config)
    : _socket(socket), _id(id), _config(config) {
  struct sockaddr addr;
  socklen_t addr_len = sizeof(addr);

  bzero(&addr, addr_len);
  _fd = accept(socket->getFd(), &addr, &addr_len);
  if (_fd == -1) {
    throw std::runtime_error("Failed to accept client");
  }

  char ip[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &((sockaddr_in *)&addr)->sin_addr, ip,
                INET_ADDRSTRLEN) == NULL) {
    throw std::runtime_error("Failed to get client ip");
  }
  _ip = ip;
  _state = C_OFF;
  if (DEBUG)
    std::cout << "Client connected : " << *this << std::endl;
}

Client::~Client(void) { close(_fd); }

Socket *Client::getSocket() const { return _socket; }

std::string Client::getServerName() const { return _serverName; }

std::string Client::getIp() const { return _ip; }

size_t Client::getId() const { return _id; }

int Client::getFd() const { return _fd; }

std::string Client::getUuid() const { return _uuid; }

void Client::setUuid(std::string const &uuid) { _uuid = uuid; }

std::string Client::getRequest() const { return _request; }

Configuration const &Client::getConfig() const { return _config; }
void Client::setState(int newState) { _state = newState; }
int Client::getState() const { return _state; }

void Client::clearRequest(void) { _request.clear(); }
std::string generateUUID() {
  char uuid[37]; // UUID is 36 characters plus null terminator
  const char *hex_chars = "0123456789abcdef";
  srand((unsigned)time(0));

  for (int i = 0; i < 8; ++i) {
    uuid[i] = hex_chars[rand() % 16];
  }
  uuid[8] = '-';
  for (int i = 9; i < 13; ++i) {
    uuid[i] = hex_chars[rand() % 16];
  }
  uuid[13] = '-';

  // UUID version 4 (4xxx)
  uuid[14] = '4';
  for (int i = 15; i < 18; ++i) {
    uuid[i] = hex_chars[rand() % 16];
  }
  uuid[18] = '-';

  // UUID variant 1 (8xxx, 9xxx, Axxx, Bxxx)
  uuid[19] = hex_chars[(rand() % 4) + 8];
  for (int i = 20; i < 23; ++i) {
    uuid[i] = hex_chars[rand() % 16];
  }
  uuid[23] = '-';

  for (int i = 24; i < 36; ++i) {
    uuid[i] = hex_chars[rand() % 16];
  }
  uuid[36] = '\0';

  return std::string(uuid);
}

int Client::recvRequest(void) {
  char buffer[BUFFER_SIZE + 1] = {0};
  int ret;

  ret = BUFFER_SIZE;
  if (_state != C_RECV)
    _request.clear();
  int flags = fcntl(_fd, F_GETFL, 0);
  fcntl(_fd, F_SETFL, flags | O_NONBLOCK);
  if (_req.findValue("transfer-encoded") != "chunked" || _state == C_RECV) {
    while (ret == BUFFER_SIZE) {
      ret = recv(_fd, buffer, BUFFER_SIZE, 0);
      if (ret > 0) {
        _request.append(buffer, ret);
        bzero(buffer, sizeof(buffer));
      }
    }
  }
  fcntl(_fd, F_SETFL, flags);
  if (_request.empty()) {
    _state = C_OFF;
    return (CLIENT_DISCONNECTED);
  }
  if (!_request.empty() && ret == -1) {
    _state = C_RECV;
    return (CLIENT_CONNECTED);
  }
  if (_req.findValue("transfer-encoding") == "chunked") {
    try {
      _req.unchunkRequest();
    } catch (std::exception const &e) {
      _res.setStatusCode(400);
      _res.setBodyError(400, this->getConfig().get_error_page(400));
      _state = C_RES;
      return (CLIENT_CONNECTED);
    }
  }
  _req.setRequest(_request);
  std::string tmp = _req.findValue("host");
  _serverName = tmp;
  if (!tmp.empty()) {
    size_t pos = _serverName.find(":");
    if (pos != tmp.npos)
      _serverName = _serverName.substr(0, pos);
  }
  _cookie.import(_req.findValue("cookie"));
  if (!_cookie.exist("uuid"))
    _cookie.insert("uuid", generateUUID());
  std::string sessionId = _cookie.find("uuid");
  if (!sessionId.empty())
      _sessionStore[sessionId]["uuid"] = sessionId;
  hashmap currentSession = getSessionById(sessionId);
  if (!currentSession.empty()) {
      _cookie.log();
      _cookie.setSession(currentSession);
      hashmap::iterator it = currentSession.begin();
      for (; it != currentSession.end(); ++it) {
          std::cout << "cs " << it->first << " => " << it->second << std::endl;
      }
  }
  _state = C_REQ;
  return (CLIENT_CONNECTED);
}

void Client::log(void) const {
  std::cout << *this << std::endl;
  std::cout << _request << std::endl;
}

std::ostream &operator<<(std::ostream &o, Client const &r) {
  o << "Client " << r.getId() << " [" << r.getState() << "] " << " ("
    << r.getFd() << ") from " << r.getIp() << " on socket " << *r.getSocket();
  if (!r.getServerName().empty())
    o << " with name " << r.getServerName();
  else
    o << " with no specified name";
  return o;
}

Cookie &Client::cookie() { return _cookie; }
void Client::checkCgi() {
  if (_state == C_CGI)
    _res.processCgi(_req, *this);
}

int Client::emptySocket() {
  char buffer[1024];
  int bytesRead;
  int bytesLeftover = 0;
  int flags = fcntl(_fd, F_GETFL, 0);
  fcntl(_fd, F_SETFL, flags | O_NONBLOCK);
  while ((bytesRead = recv(_fd, buffer, sizeof(buffer), 0)) > 0) {
    bytesLeftover += bytesRead;
  }
  fcntl(_fd, F_SETFL, flags);
  return bytesLeftover;
}

void Client::handleResponse() {
  if (_state == C_CGI) {
    _res.processCgi(_req, *this);
    return;
  } else if (_state == C_REQ) {
    _res.processRequest(_req, *this);
    return;
  }
  if (_state == C_RES) {
    if (emptySocket()) {
      _res.setStatusCode(500);
      _res.setBodyError(500, errPage(*this, 500));
    }
    _res.sendResponse(_fd);
    _res.clear();
    _req.clear();
    setState(C_OFF);
  }
  return;
}

void Client::setConfig(std::vector<Configuration> const &conf) {
  std::string host;
  hashmap::const_iterator it = _req.headers().find("host");
  if (it == _req.headers().end())
    host = "127.0.0.1";
  else {
    host = it->second;
    size_t pos = host.find(":");
    host = host.substr(0, pos);
  }
  Configuration const *first = NULL;
  for (size_t i = 0; i < conf.size(); ++i) {
    if (conf[i].get_value("host")[0] == this->_socket->getIp() &&
        strtod(conf[i].get_value("listen")[0].c_str(), NULL) ==
            this->_socket->getPort()) {
      if (!first) {
        first = &conf[i];
      }
      std::vector<std::string> tmp = conf[i].get_value("server_name");
      if (!tmp.empty() && tmp[0] == host) {
        this->_serverName = host;
        this->_config = conf[i];
        return;
      }
    }
  }
  if (first) {
    this->_config = *first;
  }
}

int Client::dataFd() { return _res.cgi().getFd(); }

hashmap Client::getSessionById(std::string const &sessionId) const {
    std::map<std::string, hashmap>::const_iterator it = _sessionStore.find(sessionId);
    if (it != _sessionStore.end())
        return  it->second;
    return hashmap();
}

void Client::setSessionValueById(
    std::string const &sessionId,
    std::pair<std::string, std::string> const &value) {
    hashmap tmp = getSessionById(sessionId);
    if (tmp.empty())
        return ;
    tmp[value.first] = value.second;
}
