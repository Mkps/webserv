#include "Client.hpp"
#include "CgiHandler.hpp"
#include "Configuration.hpp"
#include "Socket.hpp"
#include <arpa/inet.h>
#include <cstring>
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
  std::cout << "Client connected : " << *this << std::endl;
}

Client::~Client(void) { close(_fd); }

Socket *Client::getSocket() const { return _socket; }

std::string Client::getServerName() const { return _serverName; }

std::string Client::getIp() const { return _ip; }

size_t Client::getId() const { return _id; }

int Client::getFd() const { return _fd; }

std::string Client::getUuid() const { return _uuid; }

void Client::setUuid(std::string const &uuid) {
    _uuid = uuid;
}

std::string Client::getRequest() const { return _request; }

Configuration const &Client::getConfig() const { return _config; }
void Client::setState(int newState) { _state = newState; }
int Client::getState() const { return _state; }

void Client::clearRequest(void) { _request.clear(); }
std::string generateUUID() {
    char uuid[37]; // UUID is 36 characters plus null terminator
    const char* hex_chars = "0123456789abcdef";
    
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
  _request.clear();
  while (ret == BUFFER_SIZE) {
    ret = recv(_fd, buffer, BUFFER_SIZE, 0);
    if (ret <= 0) {
      return (CLIENT_DISCONNECTED);
    } else {
      buffer[ret] = 0;
      _request.append(buffer, ret);
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

Cookie &Client::cookie() {
    return _cookie;
}
void Client::checkCgi() {
  if (_state == C_CGI)
    _res.processCgi(_req, *this);
}

void Client::handleResponse() {
  if (_state == C_CGI) {
    _res.processCgi(_req, *this);
    std::cout << "HR CGI" << std::endl;
    return;
  } else if (_state == C_REQ) {
    _res.processRequest(_req, *this);
    std::cout << "HR REQ" << std::endl;
    return;
  }
  if (_state == C_RES) {
    _res.sendResponse(_fd);
    _res.clear();
    std::cout << "HR RES" << std::endl;
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
        std::cout << "found first " << _socket->getIp() << " p "
                  << _socket->getPort() << std::endl;
        first = &conf[i];
      }
      std::vector<std::string> tmp = conf[i].get_value("server_name");
      if (!tmp.empty() && tmp[0] == host) {
        std::cout << "set config for client serving " << host << std::endl;
        this->_serverName = host;
        std::cout << "conf root before " << conf[i].get_value("root")[0]
                  << std::endl;
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
