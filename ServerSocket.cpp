#include "ServerSocket.hpp"
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <string>

ServerSocket::ServerSocket(std::string IPAddress, int portNumber)
	:ASocket(IPAddress, portNumber)
{
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

ServerSocket::~ServerSocket()
{
	if (_socket >= 0)
		close(_socket);
}

int const & ServerSocket::getFd() const
{
	return _socket;
}
void	ServerSocket::log(std::string const & msg) const
{
	std::cout << msg << std::endl;
}

void ServerSocket::startListen()
{
    if (listen(_socket, 20) < 0)
    {
		std::cerr << "ServerSocket listen failed" << std::endl;
    }    std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: localhost" 
        << " PORT: " << ntohs(_socketAddr.sin_port) 
        << " ***\n\n";
    log(ss.str());
}
