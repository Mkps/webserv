#include "Client.hpp"
#include "Socket.hpp"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

Client::Client(Socket *socket, size_t id): _socket(socket), _id(id)
{
	struct sockaddr	addr;
	socklen_t		addr_len = sizeof(addr);

	bzero(&addr, addr_len);
	_fd = accept(socket->getFd(), &addr, &addr_len);
	if (_fd == -1)
	{
		throw std::runtime_error("Failed to accept client");
	}

	char ip[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &((sockaddr_in *)&addr)->sin_addr, ip, INET_ADDRSTRLEN) == NULL)
	{
		throw std::runtime_error("Failed to get client ip");
	}
	_ip = ip;
	std::cout << "Client connected : " << *this << std::endl;
}

Client::~Client(void) { close(_fd); }

Socket *Client::getSocket() const { return _socket; }

std::string	Client::getIp() const { return _ip; }

size_t	Client::getId() const { return _id; }

int	Client::getFd() const { return _fd; }

std::string	Client::getRequest() const { return _request; }

void Client::clearRequest(void) { _request.clear(); }

int	Client::recvRequest(void)
{
	char	buffer[BUFFER_SIZE + 1] = {0};
	int		ret;

	ret = BUFFER_SIZE;
	_request.clear();
	while (ret == BUFFER_SIZE)
	{
		ret = recv(_fd, buffer, BUFFER_SIZE, 0);
		if (ret <= 0)
		{
			return (CLIENT_DISCONNECTED);
		}
		else
		{
			buffer[ret] = 0;
			_request.append(buffer);
		}
	}
	_req.setRequest(_request);
	return (CLIENT_CONNECTED);
}

void	Client::log(void) const
{
	std::cout << *this << std::endl;
	std::cout << _request << std::endl;
}

std::ostream & operator<<(std::ostream & o, Client const & r)
{
	o << "Client " << r.getId() << " (" << r.getFd() << ") from " << r.getIp() << " on socket " << *r.getSocket();
	return o;
}

void	Client::handleResponse(){
	_res.processRequest(_req);
	_res.sendResponse(_fd);
	return ;
}
