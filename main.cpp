/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/14 16:28:38 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/14 16:28:39 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "Error: Incorrect argument count" << std::endl;
		return 1;
	}
	Server	serv(av[1]);
	return 0;
}
