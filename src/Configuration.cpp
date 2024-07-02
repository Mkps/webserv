/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 23:53:18 by yzaoui            #+#    #+#             */
/*   Updated: 2024/07/02 20:24:56 by yzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"


Configuration::Configuration():
_id(size_t())
{
	(void)_id;
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


/// @brief Constructeur parametric de Configuration
/// @param file_config un fichier de configuration avec la norme 
/// @param id du serveur, qui est attitré
Configuration::Configuration(std::string file_config, size_t id):
_id(id)
{
	(void)file_config;
	std::cout << "Pouvoir lire le file config sinon lire le chemin par default" << std::endl;
	std::cout << "puis ajouter les elment qui sont mis" << std::endl;
}
