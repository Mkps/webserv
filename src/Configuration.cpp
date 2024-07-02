/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 23:53:18 by yzaoui            #+#    #+#             */
/*   Updated: 2024/07/02 00:56:53 by yzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"

Configuration::Configuration():
_id(size_t())
{
}

Configuration& Configuration::operator=(const Configuration & src)
{
	if (this != &src)
	{
	}
	return (*this);
}

Configuration::Configuration(const Configuration & src):
_id(size_t())
{
	*this = src;
}

Configuration::~Configuration()
{
}

////////////////////////////////////////////////////////////////////////

Configuration::Configuration(std::string file_config, size_t id):
_id(id)
{
	std::cout << "file config je dois pouvoir lire est stockée les configuration" << std::endl;
}
