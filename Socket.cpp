#include "Socket.hpp"
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <string>

Socket::Socket(std::string IPAddress, int portNumber)
{
	(void)IPAddress;
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0) {
		//throw SocketCreationException();
		std::cerr << "socket creation exception" << std::endl;
		return ;
	}        
	_socketAddr.sin_family = AF_INET;
	_socketAddr.sin_port = htons(portNumber);
	_socketAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_socket, (sockaddr *)&_socketAddr, sizeof(_socketAddr)) < 0) {
		std::cerr << "Error creating socket" << std::endl;
	}
	startListen();
	int clientSocket = accept(_socket, NULL, NULL);
	char buf[1024] = {0};
	recv(clientSocket, buf, sizeof(buf), 0);
	std::cout << "message from client --- \n" <<  buf << " ---" << std::endl;
	std::ifstream file("resources/simple.html");
	std::string line;
	std::string msg("HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 94\n\n");
	send(clientSocket, msg.c_str(), strlen(msg.c_str()), 0);
	int calc = 0;
	while (getline(file, line)) {
		send(clientSocket, line.c_str(), strlen(line.c_str()), 0);
		calc += strlen(line.c_str());
	}
	std::cerr << "content length " << calc << std::endl;
	file.close();
}

Socket::~Socket()
{
	if (_socket >= 0)
		close(_socket);
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
