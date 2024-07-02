/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 20:23:45 by yzaoui            #+#    #+#             */
/*   Updated: 2024/07/02 16:26:17 by yzaoui           ###   ########.fr       */
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
	const size_t			_id;
	Configuration();
public:
	Configuration(std::string file_config, size_t id);
	Configuration& operator= (const Configuration & src);
	Configuration(const Configuration & src);
	~Configuration();
};

#endif