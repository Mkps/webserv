/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obouhlel <obouhlel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 20:23:45 by yzaoui            #+#    #+#             */
/*   Updated: 2024/07/03 12:10:41 by obouhlel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include <map>
#include <iomanip>
#include <iostream>

/// @brief Contient les configurations pour chaque serveur.
/// comment en parsant le fichier parse config et stockan les information des des map.
class Configuration
{
private:
	const size_t			_id;
public:
	Configuration(); 
	Configuration(std::string file_config, size_t id);
	Configuration& operator= (const Configuration & src);
	Configuration(const Configuration & src);
	~Configuration();
};

#endif