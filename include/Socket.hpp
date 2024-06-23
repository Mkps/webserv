/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obouhlel <obouhlel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:26 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/22 10:33:37 by obouhlel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <iostream>
# include <netinet/in.h>

#define MAX_CONNECTIONS 10

class Socket {
	public:
		Socket(std::string ip, short port);
		~Socket();

		int			getFd() const;
		std::string	getIp() const;
		short		getPort() const;

	private:
		int			_socketFd;
		sockaddr_in	_addr;
		std::string	_ip;
		short		_port;
};

std::ostream & operator<<(std::ostream & o, Socket const & r);

#endif