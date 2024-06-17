/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:50:49 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/17 14:51:48 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <string>
#include "Socket.hpp"

class ClientSocket : public Socket{
	public:
		ClientSocket(std::string IPAddress, int portNumber);
		~ClientSocket();
		
		void		startListen();
		void		log(std::string const & msg) const;
		int const &	getFd() const;
	private:
		ClientSocket();

		int			_socket;
		sockaddr_in _socketAddr;
};
