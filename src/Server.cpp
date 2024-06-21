/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obouhlel <obouhlel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:12 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/21 12:15:52 by obouhlel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Socket.hpp"
#include "Client.hpp"
#include <iostream>
#include <poll.h>

Server::Server(std::string config) : _nb_clients(0), _nb_sockets(0)
{
	(void)config;
	std::cout << "Supposed to open the config here" << std::endl;
	_createSocket("127.0.0.1", 8000);
	_createSocket("127.0.0.1", 8001);
}

Server::~Server()
{
	for (size_t i = 0; i < _sockets.size(); i++)
	{
		delete _sockets[i];
	}
	_sockets.clear();
	for (size_t i = 0; i < _clients.size(); i++)
	{
		delete _clients[i];
	}
	_clients.clear();
}

std::vector<s_pollfd>	Server::getPollfds() const
{
	return _pollfds;
}

std::vector<Socket *>		Server::getSockets() const
{
	return _sockets;
}

std::vector<Client *>		Server::getClients() const
{
	return _clients;
}

Socket	*Server::_createSocket(std::string ip, int port)
{
	Socket	*socket = new Socket(ip, port);
	_sockets.push_back(socket);
	_pollfds.push_back({socket->getFd(), POLLIN, 0});
	_nb_sockets++;
	return socket;
}

Client	*Server::_createClient(Socket *socket)
{
	static size_t	id = 0;
	Client			*client = new Client(socket, id++);
	s_pollfd		pollfd = {client->getFd(), POLLIN, 0};

	_clients.push_back(client);
	_pollfds.push_back(pollfd);
	_nb_clients++;
	return client;
}

void	Server::_deleteSocket(Socket *socket)
{
	for (size_t i = 0; i < _sockets.size(); i++)
	{
		if (_sockets[i] == socket)
		{
			_sockets.erase(_sockets.begin() + i);
			_pollfds.erase(_pollfds.begin() + i);
			_nb_sockets--;
			delete socket;
			break;
		}
	}
}

void	Server::_deleteClient(Client *client)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i] == client)
		{
			_clients.erase(_clients.begin() + i);
			_pollfds.erase(_pollfds.begin() + i + _nb_sockets);
			_nb_clients--;
			delete client;
			break;
		}
	}
}

void					Server::run()
{
	std::cout << "Server is running" << std::endl;
	int		ret = 0;

	while (true)
	{
		ret = poll(_pollfds.data(), _pollfds.size(), -1);
		if (ret == -1)
		{
			std::cerr << "Poll failed" << std::endl;
			break;
		}
		for (size_t i = 0; i < _nb_sockets; i++)
		{
			if (_pollfds[i].revents & POLLIN)
			{
				std::cout << "New connection" << std::endl;
				_createClient(_sockets[i]);
			}
		}
		for (size_t i = _nb_sockets; i < _pollfds.size(); i++)
		{
			if (_pollfds[i].revents & POLLIN)
			{
				std::cout << "New data recivied" << std::endl;
			}
		}
	}
}