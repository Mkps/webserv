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
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <string>
#include "Request.hpp"

Socket::Socket(std::string IPAddress, int portNumber)
{
	(void)IPAddress;
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
	startListen();
	int clientSocket = accept(_socket, NULL, NULL);
	char buf[1024] = {0};
	recv(clientSocket, buf, sizeof(buf), 0);
	Request rq(buf);
	std::cout << "method extracted " << rq.getRequestLine().getMethod() << std::endl;
	std::cout << "message from client --- \n" <<  buf << " ---" << std::endl;
	std::ifstream file("resources/simple.html");
	std::string line;
	std::string msg("HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 94\n\n");
	send(clientSocket, msg.c_str(), strlen(msg.c_str()), 0);
	while (getline(file, line)) {
		send(clientSocket, line.c_str(), strlen(line.c_str()), 0);
	}
	file.close();
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
    if (listen(_socket, 20) < 0)
    {
		std::cerr << "Socket listen failed" << std::endl;
    }    std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: localhost" 
        << " PORT: " << ntohs(_socketAddr.sin_port) 
        << " ***\n\n";
    log(ss.str());
}
