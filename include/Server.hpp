/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obouhlel <obouhlel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 11:22:28 by obouhlel          #+#    #+#             */
/*   Updated: 2024/07/03 11:38:22 by obouhlel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "./Configuration.hpp"

#include <string>
#include <vector>

#define NEW_CLIENT_CONNECTED 2

typedef struct pollfd s_pollfd;

class Socket;
class Client;

class Server {
	public:
		Server(std::string config);
		~Server();

		std::vector<s_pollfd>	getPollfds() const;
		std::vector<Socket *>	getSockets() const;
		std::vector<Client *>	getClients() const;

		void					printPollfds(void) const;
		void					updateEventPollfds(int fd, short events);

		static void				signalHandler(int signal);
		void					run();

	private:
		std::vector<s_pollfd>	_pollfds;
		std::vector<Socket *>	_sockets;
		std::vector<Client *>	_clients;
		size_t					_id;
		Configuration			_config;

		static size_t			_nbrOFServ;
		static Server			*_instance;

		Server(void);

		int						_handleNewConnection(void);
		int						_handleClientsEvent(void);
		void					_handleClientResponse(Client *client);
		int						_handleClientRequest(Client *client);
	

		Socket					*_createSocket(std::string ip, int port);
		void					_deleteSocket(Socket *socket);
		Client					*_createClient(Socket *socket);
		void					_deleteClient(Client *client);
	
	protected:
		void					closeServer(void);
};

#endif
