/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/13 15:39:17 by yzaoui            #+#    #+#             */
/*   Updated: 2024/07/13 15:42:59 by yzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

void	add_key_value(std::string str, std::map<std::string, std::vector<std::string> > &param)
{
	size_t i = 0;

	for (; str[i] != ' ' && str[i] != '\t' && i < str.length(); i++){}

	std::string key = str.substr(0, i);
	for (; (str[i] == ' ' || str[i] == '\t') && i < str.length(); i++){}

	std::string value = "";
	value = str.substr(i);
	// std::cout << "key = " << key << " | value = " << value << std::endl;
	param[key].push_back(value);
}

////////////////////////////////////////////////////////////////////////

Location::Location()
{
}

Location::Location(std::string type_of_location, std::string path, std::string config_location_str):
_type_of_location(type_of_location),
_path(path),
_param()
{
	size_t	i = 1;
	size_t	start = 1;
	for (; i < config_location_str.length() - 1; i++)
	{
		if (config_location_str[i] == ';')
		{
			add_key_value(config_location_str.substr(start, i - start), this->_param);
			start = i + 1;
		}
	}
}

Location& Location::operator=(const Location & src)
{
	if (this != &src)
	{
		this->_param = src._param;
		this->_path = src._path;
		this->_type_of_location = src._type_of_location;
	}
	return (*this);
}

Location::Location(const Location & src)
{
	*this = src;
}

Location::~Location()
{
}

////////////////////////////////////////////////////////////////////////////////

std::string					Location::get_type_of_location(void) const
{
	return (this->_type_of_location);
}

std::string					Location::get_path(void) const
{
	return (this->_path);
}

std::vector<std::string>	Location::get_value(std::string key)
{
	std::map<std::string, std::vector<std::string> >::const_iterator it = this->_param.find(key);

	if (it != _param.end())
		return it->second;
	else
		return std::vector<std::string>();
}

std::vector<std::string>	Location::get_all_key(void)
{
	std::vector<std::string>	all_key;

	for (std::map<std::string, std::vector<std::string> >::iterator it = this->_param.begin(); it !=  this->_param.end(); ++it)
		all_key.push_back(it->first);
	return (all_key);
}

void						Location::show(void)
{
	std::vector<std::string> all_key = this->get_all_key();

	std::cout << YELLOW << this->get_type_of_location() << " " << GREEN << this->get_path()  << NOCOLOR << " {" << std::endl;
	for (std::vector<std::string>::const_iterator it_key = all_key.begin(); it_key != all_key.end(); it_key++)
	{
		std::vector<std::string>	values = this->get_value(*it_key);
		std::cout << "\t" << RED << *it_key << NOCOLOR " = ";
		for (std::vector<std::string>::const_iterator it_value = values.begin(); it_value != values.end(); it_value++)
			std::cout << "\"" << BLUE << *it_value << NOCOLOR "\" ";
		std::cout << ";" << std::endl;
	}
	std::cout << "};" << std::endl;
}
