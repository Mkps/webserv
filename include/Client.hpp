#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Configuration.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Cookie.hpp"
#include <iostream>

class Socket;

#define BUFFER_SIZE 1024

enum e_reqest_state { C_OFF, C_REQ, C_CGI, C_RES };

enum e_client_state { CLIENT_CONNECTED, CLIENT_DISCONNECTED };

class Client {
public:
  //		Client(Socket *socket, size_t id);
  Client(Socket *socket, size_t id, Configuration _config);
  ~Client(void);

  Socket *getSocket() const;
  std::string getIp() const;
  size_t getId() const;
  int getFd() const;
  std::string getRequest() const;
  std::string getServerName() const;
  void setState(int newState);
  int getState() const;
  Configuration const &getConfig() const;
  void clearRequest(void);
  int dataFd();
  void checkCgi();
  std::string getUuid() const;
  void setUuid(std::string const &uuid);
  void setConfig(std::vector<Configuration> const &conf);
  Cookie &cookie();

  int recvRequest();
  void handleResponse();
  void log(void) const;

private:
  Socket *_socket;
  Request _req;
  Response _res;
  size_t _id;
  int _fd;
  int _state;
  std::string _uuid;
  std::string _ip;
  std::string _request;
  std::string _serverName;
  Cookie _cookie;
  Configuration _config;
};

std::ostream &operator<<(std::ostream &o, Client const &r);

#endif
