/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 15:15:23 by obouhlel          #+#    #+#             */
/*   Updated: 2024/09/19 20:57:29 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include "http_utils.hpp"
#include <exception>
#include <iostream>
#include <poll.h>
#include <signal.h>
#include <stdexcept>
#include <stdlib.h>
#include <unistd.h>

Server *Server::_instance = NULL;
size_t Server::_nbrOFServ = 1;

Server::Server(std::string config) : _id(Server::_nbrOFServ) {
  (void)config;
  _continue = 1;
  Server::_nbrOFServ++;
  void *ptr = NULL;

  ptr = _createSocket("127.0.0.1", 8000);
  if (!ptr) {
    throw std::runtime_error("Failed to create socket");
  }

  ptr = _createSocket("127.0.0.1", 8001);
  if (!ptr) {
    throw std::runtime_error("Failed to create socket");
  }
}

Server::Server(std::vector<Configuration> const &vConf) : _id(vConf.size()) {
  _config = vConf;
  _continue = 1;
  Server::_nbrOFServ++;
  void *ptr = NULL;
  for (size_t i = 0; i < vConf.size(); ++i) {
    std::string host;
    int port;
    if (vConf[i].get_value("host").empty())
      host = "127.0.0.1";
    else
      host = vConf[i].get_value("host")[0];
    if (vConf[i].get_value("listen").empty())
      port = 80;
    else
      port = strtod(vConf[i].get_value("listen")[0].c_str(), NULL);
    if (!socketExists(host, port))
      ptr = _createSocket(host, port);
    if (!ptr) {
      throw std::runtime_error("Failed to create socket");
    }
  }
}

Server::~Server(void) {
  std::cerr << "\tQuitting" << std::endl;
  Server::_nbrOFServ--;
  for (size_t i = 0; i < _sockets.size(); i++) {
    delete _sockets[i];
  }
  _sockets.clear();
  for (size_t i = 0; i < _clients.size(); i++) {
    delete _clients[i];
  }
  _clients.clear();
}

bool Server::socketExists(std::string host, int port) const {
  for (size_t i = 0; i < _sockets.size(); ++i) {
    if (_sockets[i]->getIp() == host && _sockets[i]->getPort() == port)
      return true;
  }
  return false;
}
std::vector<s_pollfd> Server::getPollfds() const { return _pollfds; }

std::vector<Socket *> Server::getSockets() const { return _sockets; }

std::vector<Client *> Server::getClients() const { return _clients; }

Socket *Server::_createSocket(std::string ip, int port) {
  Socket *s = NULL;
  try {
    s = new Socket(ip, port);
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return NULL;
  }

  s_pollfd pollfd = {s->getFd(), POLLIN | POLLOUT, 0};

  _sockets.push_back(s);
  _pollfds.push_back(pollfd);

  return s;
}

Client *Server::_createClient(Socket *socket, Configuration const &conf) {
  static size_t id = 0;

  Client *client = NULL;
  try {
    client = new Client(socket, id++, conf);
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return NULL;
  }

  s_pollfd pollfd = {client->getFd(), POLLIN | POLLOUT, 0};
  _clients.push_back(client);
  _pollfds.push_back(pollfd);

  return client;
}

void Server::_deleteSocket(Socket *socket) {
  for (size_t i = 0; i < _sockets.size(); i++) {
    if (_sockets[i] == socket) {
      _sockets.erase(_sockets.begin() + i);
      _pollfds.erase(_pollfds.begin() + i);
      delete socket;
      break;
    }
  }
}

void Server::_deleteClient(Client *client) {
  for (size_t i = 0; i < _clients.size(); i++) {
    if (_clients[i] == client) {
      _clients.erase(_clients.begin() + i);
      _pollfds.erase(_pollfds.begin() + i + _sockets.size());
      delete client;
      break;
    }
  }
}

void Server::signalHandler(int signal) {
  (void)signal;
  _instance->_continue = 0;
}

int Server::_handleNewConnection(void) {
  void *ptr = NULL;

  for (size_t i = 0; i < _sockets.size(); i++) {
    if (!(_pollfds[i].revents & POLLIN))
      continue;
    if (DEBUG)
      std::cout << "\tNew connection on " << *_sockets[i] << std::endl;
    ptr = _createClient(_sockets[i], _config[i]);
    if (!ptr)
      return (EXIT_FAILURE);
    return (NEW_CLIENT_CONNECTED);
  }
  return (EXIT_SUCCESS);
}

void Server::_handleEventIn(Client &client, int const &i) {
  int ret = 0;
  if (client.getState() >= C_OFF && client.getState() <= C_RECV) {
    ret = _handleClientRequest(&client);
    if (ret == CLIENT_DISCONNECTED)
      return;
    client.setState(C_REQ);
  }
  if (client.getRequest().empty() && client.getState() == C_REQ) {
    client.setState(C_OFF);
    return;
  }
  if (client.getState() >= C_RECVH && client.getState() <= C_RECV) {
    return;
  }
  client.handleRequest();
  _pollfds[i].events = POLLIN | POLLOUT;
}

int Server::_handleClientsEvent(void) {
  void *ptr = NULL;

  for (size_t i = _sockets.size(); i < _pollfds.size(); i++) {
    if (_pollfds[i].revents & POLLIN) {
      ptr = _clients[i - _sockets.size()];
      if (!ptr) {
        std::cerr << "Failed to get client" << std::endl;
        return (EXIT_FAILURE);
      }
      Client *client = static_cast<Client *>(ptr);
 	  if (!client)
	  	return (EXIT_FAILURE);
      _handleEventIn(*client, i);
	  return EXIT_SUCCESS;
    }
    if (_pollfds[i].revents & POLLOUT) {
      ptr = _clients[i - _sockets.size()];
      if (!ptr) {
        std::cerr << "Failed to get client" << std::endl;
        return (EXIT_FAILURE);
      }
      Client *client = static_cast<Client *>(ptr);
	  if (!client)
	  	return (EXIT_FAILURE);
      if (client->getState() == C_RES)
        client->handleResponse();
      else if (client->getState() == C_OFF)
        _pollfds[i].events = POLLIN;
  	  return EXIT_SUCCESS;

    }
    if (_pollfds[i].revents & (POLLERR | POLLNVAL | POLLHUP)) {
      ptr = _clients[i - _sockets.size()];
      if (!ptr) {
        std::cerr << "Failed to get client" << std::endl;
        return (EXIT_FAILURE);
      }
      Client *client = static_cast<Client *>(ptr);
	  if (!client)
	  	return (EXIT_FAILURE);
      if (client->getState() == C_RES) {
        client->handleError();
        _pollfds[i].events = POLLIN;
      }
  	  return EXIT_SUCCESS;
    }
  }
  return (EXIT_SUCCESS);
}

int Server::_handleClientRequest(Client *client) {
  int ret = 0;

  ret = client->recvRequest();
  if (client->getState() >= C_RECVH && client->getState() <= C_RECV) {
    return CLIENT_DISCONNECTED;
  }
  if (ret == CLIENT_DISCONNECTED) {
    if (DEBUG)
      std::cout << "Client disconnected : " << *client << std::endl;
    _deleteClient(client);
    return ret;
  }
  client->setConfig(_config);
  return (ret);
}

int Server::_handleTimeout() {
  for (size_t i = 0; i < _clients.size(); ++i) {
    int state = _clients[i]->getState();
    if (state == C_CGI) {
      _clients[i]->checkCgi();
    } else if (state > C_OFF && state < C_RES) {
      _clients[i]->checkTimeout();
    }
  }
  return 0;
}

int Server::_handleSend() {
  for (size_t i = 0; i < _clients.size(); ++i) {
    int state = _clients[i]->getState();
    // Unneeded?
    /*if (state == C_RECV) {*/
    /*  _handleClientRequest(_clients[i]);*/
    /*} else if (state == C_REQ) {*/
    /*  _handleEventIn(*_clients[i], i);*/
    /*}*/
    if (state == C_RES) {
      _clients[i]->handleResponse();
    }
  }
  return 0;
}

void Server::_handleClientResponse(Client *client) { client->handleResponse(); }

void Server::run() {
  int ret = 0;
  int timeout = 150;

  _instance = this;
  signal(SIGINT, Server::signalHandler);
  signal(SIGPIPE, SIG_IGN);
  while (_continue) {
    ret = poll(_pollfds.data(), _pollfds.size(), timeout);
    if (ret == -1) {
      break;
    } else if (ret == 0) {
      _handleTimeout();
    } else {
      ret = _handleNewConnection();
      if (ret == EXIT_FAILURE)
        break;
      else if (ret == NEW_CLIENT_CONNECTED)
        continue;
    }
    if (_handleClientsEvent() == EXIT_FAILURE) {
      break;
    }
    if (_handleSend() == EXIT_FAILURE) {
      break;
    }
    _handleTimeout();
  }
}
