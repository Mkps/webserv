#include "Socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <unistd.h>

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
	std::cout << "message from client --- " <<  buf << " ---" << std::endl;
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
