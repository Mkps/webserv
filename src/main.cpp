/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/14 16:28:38 by aloubier          #+#    #+#             */
/*   Updated: 2024/07/10 16:47:14 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Configuration.hpp"

#include <iostream>
#include <stdexcept>

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
		for (std::vector<Configuration>::iterator it = ConfigurationForAllServ.begin(); it != ConfigurationForAllServ.end(); it++)
		{
			(*it).show();
			std::cout << "----------" << std::endl;
		}
		
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
