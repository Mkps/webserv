/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/14 16:28:38 by aloubier          #+#    #+#             */
/*   Updated: 2024/07/02 20:44:05 by yzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"

#define NOCOLOR "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"


int	main(int ac, char **av)
{
	const std::string defaultfile = "configurations/default.config";
	try
	{
		std::string	fileConfig = defaultfile;
		if (ac > 2)
			throw std::invalid_argument("Incorrect argument count");
		else if (ac == 2)
			fileConfig = av[1];
		Server *server = new Server(fileConfig);
		server->run();
		delete server;
	}
	catch(const std::exception& e)
	{
		std::cerr <<RED "Error: " NOCOLOR << e.what() << '\n';
		return (1);
	}
	return 0;
}
