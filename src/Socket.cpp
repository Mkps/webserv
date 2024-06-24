/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obouhlel <obouhlel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:28 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/22 10:54:09 by obouhlel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <stdexcept>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/socket.h>

Socket::Socket(std::string ip, short port) : _ip(ip), _port(port)
{
	int opt = 1;

	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd == -1)
	{
		throw std::runtime_error("Failed to create socket");
	}

	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
	{
		throw std::runtime_error("Failed to reuse socket address");
	}

	bzero(&_addr, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	_addr.sin_port = htons(port);

	if (bind(_socketFd, (sockaddr *)&_addr, sizeof(_addr)) == -1)
	{
		throw std::runtime_error("Failed to bind socket");
	}

	if (listen(_socketFd, MAX_CONNECTIONS) == -1)
	{
		throw std::runtime_error("Failed to listen on socket");
	}

	std::cout << "Socket created : " << *this << std::endl;
}

Socket::~Socket(void) { close(_socketFd); }

int Socket::getFd() const { return _socketFd; }

std::string	Socket::getIp() const { return _ip; }

short Socket::getPort() const { return _port; }

std::ostream & operator<<(std::ostream & o, Socket const & r)
{
	o << r.getIp() << ":" << r.getPort();
	return o;
}