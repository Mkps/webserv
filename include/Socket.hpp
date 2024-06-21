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
#include "Request.hpp"
#include "Response.hpp"
class Socket {
	public:
		Socket(std::string IPAddress, int portNumber);
		~Socket();
		
		void		log(std::string const & msg) const;
		int const &	getFd() const;
	private:
		Socket();
		void		startListen();
		void		sendResponse();
		void		initSocket(int portNumber);
  		void		processRequest(Request const & req, Response & resp);

		int			_socket;
		sockaddr_in _socketAddr;
		Request		_request;
		Response	_response;

};
#endif
