/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/14 16:28:38 by aloubier          #+#    #+#             */
/*   Updated: 2024/07/05 17:55:56 by yzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"
#include "Configuration.hpp"


int	main(int ac, char **av)
{
	const std::string defaultfile = "configurations/minimal.config";
	try
	{
		std::string	fileConfig = defaultfile;
		if (ac > 2)
			throw std::invalid_argument("Incorrect argument count.");
		else if (ac == 2)
			fileConfig = av[1];
		std::vector<Configuration>	ConfigurationForAllServ = getAllConf(fileConfig);

		// Server *server = new Server(fileConfig);
		// server->run();
		// delete server;
	}
	catch(const std::exception& e)
	{
		std::cerr <<RED "Error: " NOCOLOR << e.what() << '\n';
		return (1);
	}
	return 0;
}
