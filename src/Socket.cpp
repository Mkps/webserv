/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:28 by aloubier          #+#    #+#             */
/*   Updated: 2024/07/05 15:51:06 by yzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdexcept>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

inline int set_socket_non_blocking(int sockfd) {
  int flags = fcntl(sockfd, F_GETFL, 0);
  if (flags == -1) {
    // Handle error
    std::cerr << "Error getting fcntl flag" << std::endl;
    return -1;
  }

  if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
    // Handle error
    std::cerr << "Error setting non-block flag" << std::endl;
    return -1;
  }

  return 0;
}

Socket::Socket(std::string ip, short port) : _ip(ip), _port(port) {
  int opt = 1;
  _socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_socketFd == -1) {
    throw std::runtime_error("Failed to create socket");
  }

  if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt)) == -1) {
    throw std::runtime_error("Failed to reuse socket address");
  }
  if (set_socket_non_blocking(_socketFd) == -1) {
    throw std::runtime_error("Failed to make socket non blocking");
  }
  bzero(&_addr, sizeof(_addr));
  _addr.sin_family = AF_INET;
  _addr.sin_addr.s_addr = inet_addr(ip.c_str());
  _addr.sin_port = htons(port);

  if (bind(_socketFd, (sockaddr *)&_addr, sizeof(_addr)) == -1) {
    throw std::runtime_error("Failed to bind socket");
  }

  if (listen(_socketFd, MAX_CONNECTIONS) == -1) {
    throw std::runtime_error("Failed to listen on socket");
  }

  std::cout << "Socket created : " << *this << std::endl;
}

Socket::~Socket(void) { close(_socketFd); }

int Socket::getFd() const { return _socketFd; }

std::string Socket::getIp() const { return _ip; }

short Socket::getPort() const { return _port; }

std::ostream &operator<<(std::ostream &o, Socket const &r) {
  o << r.getIp() << ":" << r.getPort();
  return o;
}
