/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:21 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/17 14:51:21 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

#include <netinet/in.h>
#include <string>

class ClientSocket;
class ServerSocket {
	public:
		ServerSocket(std::string IPAddress, int portNumber);
		~ServerSocket();
		
		void		startListen();
		void		log(std::string const & msg) const;
		int const &	getFd() const;
	private:
		ServerSocket();

		int			_socket;
		sockaddr_in _socketAddr;
		ClientSocket* clientArray;

};
#endif
