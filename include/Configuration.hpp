/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 20:23:45 by yzaoui            #+#    #+#             */
/*   Updated: 2024/07/05 17:56:01 by yzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include <map>
#include <vector>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm> // Pour std::remove

#define NOCOLOR "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define PURPLE "\033[35m"

/// @brief class contenant les configuration en fonction des location
class Location
{
private:
	std::string											_type_of_location;
	std::string											_path;
	std::map<std::string, std::vector<std::string> >	_param;
	Location();
public:
	Location(std::string type_of_location, std::string path, std::string config_location_str);
	Location& operator= (const Location & src);
	Location(const Location & src);
	~Location();

////////////////////////////////////////
	std::string					get_type_of_location(void) const;
	std::string					get_path(void) const;
	std::vector<std::string>	get_value(std::string key);
	std::vector<std::string>	get_all_key(void);
	void						show(void);
};


/// @brief Contient les configurations pour chaque serveur.
/// comment en parsant le fichier parse config et stockan les information des des map.
class Configuration
{
private:
	const size_t										_id;					// id du serveur chaque serveur aura un id unique
	std::map<std::string, std::vector<std::string> >	_param;
	std::vector<Location>								_locations;
	Configuration();
	void	_add_location(const std::string);
public:
	Configuration(std::string config_str, size_t id);
	Configuration& operator= (const Configuration & src);
	Configuration(const Configuration & src);
	~Configuration();

////////////////////////////////////////
	size_t						get_id(void) const;
	std::vector<std::string>	get_value(std::string key) const;
	std::vector<std::string>	get_all_key(void);
	std::vector<Location>		get_locations(void) const;
	void						show(void);
};

std::vector<Configuration> getAllConf(std::string file_config);

#endif