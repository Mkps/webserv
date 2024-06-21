/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:28 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/17 14:51:29 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <asm-generic/socket.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <unistd.h>
#include <string>
#include <vector>
#include "Request.hpp"
#include "Response.hpp"

Socket::Socket(std::string IPAddress, int portNumber)
{
	(void)IPAddress;
	initSocket(portNumber);
	startListen();
	int clientSocket = accept(_socket, NULL, NULL);
	char buf[1024] = {0};
	recv(clientSocket, buf, sizeof(buf), 0);
	std::cout << "message from client --- \n" <<  buf << " ---" << std::endl;
	Request rq(buf);
	Response r;
	r.processRequest(rq);
	r.sendResponse(clientSocket);
}

Socket::~Socket()
{
	if (_socket >= 0)
		close(_socket);
}

int const & Socket::getFd() const
{
	return _socket;
}

void	Socket::log(std::string const & msg) const
{
	std::cout << msg << std::endl;
}

void Socket::startListen()
{
    if (listen(_socket, 20) < 0) {
		std::cerr << "Socket listen failed" << std::endl;
    }    std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: localhost" 
        << " PORT: " << ntohs(_socketAddr.sin_port) 
        << " ***\n\n";
    log(ss.str());
}
void	Socket::initSocket(int portNumber)
{
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0) {
		//throw SocketCreationException();
		std::cerr << "socket creation exception" << std::endl;
		return ;
	}        
	const int enable = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(int)))
	{
		std::cerr << "socket configuration exception" << std::endl;
		return ;
	}
	_socketAddr.sin_family = AF_INET;
	_socketAddr.sin_port = htons(portNumber);
	_socketAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_socket, (sockaddr *)&_socketAddr, sizeof(_socketAddr)) < 0) {
		perror("Error: ");
		std::cerr << "Error creating socket" << std::endl;
	}
}

