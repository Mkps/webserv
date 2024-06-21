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
	processRequest(rq, r);
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
#include <sys/stat.h>
inline int fileExists(const std::string & name) {
	struct stat buf;
	if (stat( name.c_str(), &buf) == 0)
	{
		std::cout << "st size " << buf.st_size << std::endl;
		return buf.st_size;
	}
	else
		return -1;
}

template <typename T>
std::string NumberToString ( T Number )
{
	std::ostringstream ss;
	ss << Number;
	return ss.str();
}

void	Socket::processRequest(Request const & req, Response & resp)
{
	int tmp;
	std::string myStr[] = {"index", "index.html"};
	std::vector<std::string>tryFiles;
	std::string root = ".";
	std::string path = root + req.getFilePath();
	tryFiles.assign(myStr, myStr + 2);
	if (!req.getFilePath().empty()  && req.getFilePath()[req.getFilePath().size() - 1] == '/')
	{
		for (std::vector<std::string>::const_iterator it = tryFiles.begin(); it != tryFiles.end(); ++it) {
			std::string filePath(path + *it);
			std::cout << "file path is " << filePath << std::endl;
			if ((tmp = fileExists(filePath)) == -1) {
				resp.setStatusCode(400);
				resp.setHeader("content-length", "24");
				resp.setHeader("content-type", "text/html");
				resp.setBodyError();
			}
			else {
				resp.setStatusCode(200);
				break;
			}
		}
	}
	else {
			std::cout << "file path is " << path << std::endl;
			if ((tmp = fileExists(path)) == -1) {
				resp.setStatusCode(404);
				resp.setHeader("content-length", "24");
				resp.setHeader("content-type", "text/html");
				resp.setBodyError();
			}
			else {
				resp.setStatusCode(200);
			}
	}
	if (resp.getStatusCode() != 200) {
		std::cerr << "couldnt find file" << std::endl;
		return;
	}
	resp.setStatusCode(200);
	std::string bodySize = NumberToString<int>(tmp);
	std::cout << bodySize << std::endl;
	resp.setHeader("content-length", "");
	resp.setHeader("content-type", "text/html");
	resp.setBody(path);
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

