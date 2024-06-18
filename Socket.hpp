/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:26 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/17 14:51:26 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <netinet/in.h>
#include <string>
class Socket {
	public:
		Socket(std::string IPAddress, int portNumber);
		~Socket();
		
		void		startListen();
		void		log(std::string const & msg) const;
		int const &	getFd() const;
	private:
		Socket();

		int			_socket;
		sockaddr_in _socketAddr;

};
#endif
