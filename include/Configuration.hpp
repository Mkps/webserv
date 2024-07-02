/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 20:23:45 by yzaoui            #+#    #+#             */
/*   Updated: 2024/07/02 20:26:18 by yzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include <map>
#include <iomanip>
#include <iostream>
#include <fstream>

/// @brief Contient les configurations pour chaque serveur.
/// comment en parsant le fichier parse config et stockan les information des des map.
class Configuration
{
private:
	const size_t						_id;					// id du serveur chaque serveur aura un id unique
	int									_port;					// le port dentre et sortie (si yen pas il doit en avoir un par defaut)
	std::string							_server_name;			// le nom du serveur (si y a pas de nom comportement indeterminer (jen donne un par defaut ?))
	std::map<std::string, std::map<std::string , std::string> >	_locations;				// j'ai pas trop compris les chemin disponible ?
	std::map<int, std::string>			_error_pages;			// les page derreur ??
	size_t								_client_max_body_size;	//la taille maximum (comportmeent indeterminer si y a pas)
	
	Configuration();
public:
	Configuration(std::string file_config, size_t id);
	Configuration& operator= (const Configuration & src);
	Configuration(const Configuration & src);
	~Configuration();
};

#endif