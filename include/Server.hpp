/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:15 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/17 14:51:15 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>

class Server {
	public:
		Server(std::string config);
		~Server();

	private:
		Server();
		Server(Server const & src);
		Server & operator= (Server const & rhs);
};
#endif //SERVER_HPP
