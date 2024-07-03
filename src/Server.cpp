/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obouhlel <obouhlel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 11:22:56 by obouhlel          #+#    #+#             */
/*   Updated: 2024/07/03 11:22:59 by obouhlel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Socket.hpp"
#include <exception>
#include <iostream>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

Server	*Server::_instance = NULL;
size_t	Server::_nbrOFServ = 1;

Server::Server(std::string config):
_id(Server::_nbrOFServ),
_config(config, this->_id)
{
	Server::_nbrOFServ++;
	std::cout << "Supposed to open the config here" << std::endl;
	void	*ptr = NULL;

	ptr = _createSocket("127.0.0.1", 8000);
	if (!ptr)
	{
		std::cerr << "Failed to create socket" << std::endl;
		delete this;
		exit(1);
	}

	ptr = _createSocket("127.0.0.1", 8001);
	if (!ptr)
	{
		std::cerr << "Failed to create socket" << std::endl;
		delete this;
		exit(1);
	}
}

Server::~Server(void)
{
	Server::_nbrOFServ--;
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

std::vector<s_pollfd> Server::getPollfds() const { return _pollfds; }

std::vector<Socket *> Server::getSockets() const { return _sockets; }

std::vector<Client *> Server::getClients() const { return _clients; }

Socket *Server::_createSocket(std::string ip, int port)
{
	Socket	*sock = NULL;
	try
	{
		sock = new Socket(ip, port);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return NULL;
	}

	s_pollfd pollfd = {
		.fd = sock->getFd(),
		.events = POLLIN,
		.revents = 0
	};

	_sockets.push_back(sock);
	_pollfds.push_back(pollfd);

	return sock;
}

Client *Server::_createClient(Socket *socket)
{
	static size_t	id = 0;

	Client	*client = NULL;
	try
	{
		client = new Client(socket, id++);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return NULL;
	}

	s_pollfd pollfd = {
		.fd = client->getFd(),
		.events = POLLIN,
		.revents = 0
	};

	_clients.push_back(client);
	_pollfds.push_back(pollfd);

	return client;
}

void Server::_deleteSocket(Socket *socket)
{
	for (size_t i = 0; i < _sockets.size(); i++)
	{
		if (_sockets[i] == socket)
		{
			_sockets.erase(_sockets.begin() + i);
			_pollfds.erase(_pollfds.begin() + i);
			delete socket;
			break;
		}
	}
}

void Server::_deleteClient(Client *client)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i] == client)
		{
			_clients.erase(_clients.begin() + i);
			_pollfds.erase(_pollfds.begin() + i + _sockets.size());
			delete client;
			break;
		}
	}
}

void Server::signalHandler(int signal)
{
	(void)signal;
	if (_instance)
	{
		delete _instance;
	}
	exit(0);
}

int Server::_handleNewConnection(void)
{
	void	*ptr = NULL;

	for (size_t i = 0; i < _sockets.size(); i++)
	{
		if (!(_pollfds[i].revents & POLLIN))
			continue;
		std::cout << "\tNew connection on " << *_sockets[i] << std::endl;
		ptr = _createClient(_sockets[i]);
		if (!ptr)
			return (EXIT_FAILURE);
		return (NEW_CLIENT_CONNECTED);
	}
	return (EXIT_SUCCESS);
}

std::string poll_event_to_string(short events)
{
	std::string ret = "";

	if (events & POLLIN)
		ret.append("POLLIN ");
	if (events & POLLPRI)
		ret.append("POLLPRI ");
	if (events & POLLOUT)
		ret.append("POLLOUT ");
	if (events & POLLRDHUP)
		ret.append("POLLRDHUP ");
	if (events & POLLERR)
		ret.append("POLLERR ");
	if (events & POLLHUP)
		ret.append("POLLHUP ");
	if (events & POLLNVAL)
		ret.append("POLLNVAL ");
	if (events & POLLRDNORM)
		ret.append("POLLRDNORM ");
	if (events & POLLRDBAND)
		ret.append("POLLRDBAND ");
	if (events & POLLWRNORM)
		ret.append("POLLWRNORM ");
	if (events & POLLWRBAND)
		ret.append("POLLWRBAND ");
	if (ret.size())
	{
		ret = ret.substr(0, ret.size() - 1);
		return ret;
	}
	return "NONE";
}

void Server::printPollfds(void) const
{
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		std::cout << "Pollfd " << i << " : " << _pollfds[i].fd << std::endl;
		std::cout << "Pollfd " << i << " : " << poll_event_to_string(_pollfds[i].events) << std::endl;
		std::cout << "Pollfd " << i << " : " << poll_event_to_string(_pollfds[i].revents) << std::endl;
	}
}

void Server::updateEventPollfds(int fd, short events)
{
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		if (_pollfds[i].fd == fd)
		{
			_pollfds[i].events = events;
			break;
		}
	}
}

int Server::_handleClientsEvent(void)
{
	void	*ptr = NULL;
	int		ret = 0;

	for (size_t i = _sockets.size(); i < _pollfds.size(); i++)
	{
		ptr = _clients[i - _sockets.size()];
		if (!ptr)
		{
			std::cerr << "Failed to get client" << std::endl;
			return (EXIT_FAILURE);
		}
		Client	*client = static_cast<Client *>(ptr);

		if (_pollfds[i].revents & (POLLERR | POLLHUP))
        {
            std::cout << "Error or hangup detected on socket" << std::endl;
            _deleteClient(client);
            continue ;
        }
		if (_pollfds[i].revents & POLLIN)
		{
			std::cout << "RECEIVED" << std::endl;
			printPollfds();
			ret = _handleClientRequest(client);
			if (ret == CLIENT_DISCONNECTED)
				continue;
			if (client->getRequest().empty())
				continue;
			updateEventPollfds(client->getFd(), POLLOUT);
		}
		if (_pollfds[i].revents & POLLOUT)
		{
			std::cout << "SENDING" << std::endl;
			printPollfds();
			_handleClientResponse(client);
			updateEventPollfds(client->getFd(), POLLIN);
		}
		// _deleteClient(client);
	}
	return (EXIT_SUCCESS);
}

int Server::_handleClientRequest(Client *client)
{
	int		ret = 0;

	ret = client->recvRequest();
	if (ret == CLIENT_DISCONNECTED)
	{
		std::cout << "Client disconnected : " << *client << std::endl;
		_deleteClient(client);
		return ret;
	}
	client->log();
	std::cout << "Request received from " << *client << std::endl;
	return (ret);
}

void Server::_handleClientResponse(Client *client)
{
	client->handleResponse();
}

void Server::run()
{
	int		ret = 0;
	int		timeout = 0;

	_instance = this;
	signal(SIGINT, Server::signalHandler);
	while (true)
	{
		timeout = -1;
		std::cout << "Polling..." << std::endl;
		printPollfds();
		ret = poll(_pollfds.data(), _pollfds.size(), timeout);
		if (ret == -1)
		{
			std::cerr << "Poll failed" << std::endl;
			break;
		}
		ret = _handleNewConnection();
		if (ret == EXIT_FAILURE)
			break;
		else if (ret == NEW_CLIENT_CONNECTED)
			continue;
		if (_handleClientsEvent() == EXIT_FAILURE)
			break;
	}
}
