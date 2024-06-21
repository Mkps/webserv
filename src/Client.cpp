#include "Client.hpp"
#include "Socket.hpp"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

Client::Client(Socket *socket, size_t id): _socket(socket), _id(id)
{
	struct sockaddr	addr = {0};
	socklen_t		addr_len = sizeof(addr);

	_fd = accept(socket->getFd(), &addr, &addr_len);
	if (_fd == -1)
	{
		perror("accept");
		return ;
	}
	char ip[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &((sockaddr_in *)&addr)->sin_addr, ip, INET_ADDRSTRLEN) == NULL)
	{
		perror("inet_ntop");
		return ;
	}
	_ip = ip;
	std::cout << "New client created" << std::endl;
}

Client::~Client()
{
	close(_fd);
	std::cout << "Client destroyed" << std::endl;
}

int	Client::getFd() const
{
	return _fd;
}

std::string	Client::getRequest() const
{
	return _request;
}

std::string	Client::getIp() const
{
	return _ip;
}

int	Client::recvRequest(void)
{
	char	buffer[BUFFER_SIZE];
	int		ret;

	ret = BUFFER_SIZE;
	while (ret == BUFFER_SIZE)
	{
		ret = recv(_fd, buffer, BUFFER_SIZE, 0);
		if (ret <= 0)
		{
			delete this;
			return (CLIENT_DISCONNECTED);
		}
		else
		{
			buffer[ret] = 0;
			_request.append(buffer);
		}
	}
	return (CLIENT_CONNECTED);
}

void	Client::log(void) const
{
	std::cout << "Client " << _ip << " [" << _fd << "] :" << std::endl;
	std::cout << _request << std::endl;
}

std::ostream & operator<<(std::ostream & o, Client const & r)
{
	o << "Client " << r.getIp() << " [" << r.getFd() << "]";
	return o;
}