/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 15:45:38 by yzaoui            #+#    #+#             */
/*   Updated: 2024/07/10 15:51:38 by yzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <map>
#include <vector>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>

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

#endif