/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocket.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:50:42 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/17 14:51:53 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <string>
class ASocket {
	public:
		ASocket(std::string IPAddress, int portNumber);
		~ASocket();
		
		void		startListen();
		void		log(std::string const & msg) const;
		int const &	getFd() const;
	private:
		ASocket();

		int			_socket;
		sockaddr_in _socketAddr;

};
