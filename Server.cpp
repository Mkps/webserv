#include "Server.hpp"
#include "Socket.hpp"
#include <iostream>

Server::Server(std::string config) {
	(void)config;
	std::cout << "Supposed to open the config here" << std::endl;
	Socket Socket("127.0.0.1", 8080);
}

Server::~Server(){
}
