/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 20:23:45 by yzaoui            #+#    #+#             */
/*   Updated: 2024/07/13 16:01:49 by yzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include "Location.hpp"

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
	std::vector<Location>		get_locations_by_path(std::string path) const;
	void						show(void);
	size_t						get_client_max_body_size(void) const;
	bool						is_a_legit_Method(const std::string &) const;
	bool						is_a_allowed_Method(const std::string &) const;
	std::string					get_path_upload(void) const;
	std::string					get_error_page(size_t error_code) const;
};
// Du genre pour recuperer le max_body_size si je suis dans upload ou recuperer directement un vecteur avec les allowed_methods.

std::vector<Configuration> getAllConf(std::string file_config);

#endif