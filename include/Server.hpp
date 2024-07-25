/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 15:14:33 by obouhlel          #+#    #+#             */
/*   Updated: 2024/07/05 17:02:43 by yzaoui           ###   ########.fr       */
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
        Server(std::vector<Configuration> const &vConf);
		~Server();

		std::vector<s_pollfd>	getPollfds() const;
		std::vector<Socket *>	getSockets() const;
		std::vector<Client *>	getClients() const;
        bool socketExists(std::string host, int port) const;

		static void				signalHandler(int signal);
		void					run();

	private:
		std::vector<s_pollfd>	_pollfds;
		std::vector<Socket *>	_sockets;
		std::vector<Client *>	_clients;
		size_t					_id;
        std::vector<Configuration>          _config;
        int _continue;
		// Configuration			_config;

		static size_t			_nbrOFServ;
		static Server			*_instance;

		Server(void);

		int						_handleNewConnection(void);
		int						_handleClientsEvent(void);
		void					_handleClientResponse(Client *client);
		int						_handleClientRequest(Client *client);
        int _handleTimeout(); 
        int _handleSend(); 
	

		Socket					*_createSocket(std::string ip, int port);
		void					_deleteSocket(Socket *socket);
		Client					*_createClient(Socket *socket, Configuration const &config);
		void					_deleteClient(Client *client);
};

#endif
