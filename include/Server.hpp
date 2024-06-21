/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obouhlel <obouhlel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:15 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/21 11:47:30 by obouhlel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <vector>

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

		void					run();

	private:
		std::vector<s_pollfd>	_pollfds;
		std::vector<Socket *>	_sockets;
		std::vector<Client *>	_clients;
		size_t					_nb_clients;
		size_t					_nb_sockets;

		Server(void);
		Socket	*_createSocket(std::string ip, int port);
		void	_deleteSocket(Socket *socket);
		Client	*_createClient(Socket *socket);
		void	_deleteClient(Client *client);
};

#endif //SERVER_HPP
