/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 23:53:18 by yzaoui            #+#    #+#             */
/*   Updated: 2024/07/05 18:46:35 by yzaoui           ###   ########.fr       */
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

// 1. Verifier qu'il sagit dun fichier .config ou .conf et qu'on peut l'ouvrire/lire sinon envoyer une exception
static void verifFileConfig(std::string file_config)
{
	std::string::size_type dot_pos = file_config.find_last_of('.');
	if (dot_pos == std::string::npos)
		throw std::runtime_error("Le fichier n'a pas d'extension: " + file_config);

	std::string extension = file_config.substr(dot_pos);
	if (extension != ".con" && extension != ".config")
		throw std::runtime_error("Le fichier doit avoir une extension .con ou .config: " + file_config);

}

/// @brief Constructeur parametric de Configuration
/// @param file_config un fichier de configuration avec la norme
/// @param id du serveur, qui est attitré
Configuration::Configuration(std::string file_config, size_t id):
_id(id)
{
	std::cout << "Pouvoir lire le file config sinon lire le chemin par default" << std::endl;
	std::cout << "file_config = " << file_config << std::endl;
	std::cout << "puis ajouter les elements qui sont mis" << std::endl << std::endl;
	std::cout << "------------------------------------" << std::endl << std::endl;
	// 1. Verifier qu'il sagit dun fichier .config ou .conf et qu'on peut l'ouvrire/lire sinon envoyer une exception
	verifFileConfig(file_config);
	std::ifstream file_stream(file_config.c_str());
	if (!file_stream.is_open() || !file_stream.good())
		throw std::runtime_error("Impossible d'ouvrir ou de lire le fichier de configuration: " + file_config);
	file_stream.close();
}

static std::string find_end_of_server_config(std::string &line, size_t &nbr_paire_braquet, bool &key_Word_Find, bool &braquet_open)
{
	std::string	in_config = std::string();
	size_t	i = 0;
	std::string::const_iterator c = line.begin();
	for (; nbr_paire_braquet && c != line.end(); c++, i++)
	{
		if (*c == '{')
			nbr_paire_braquet++;
		else if (*c == '}')
			nbr_paire_braquet--;
		if (!nbr_paire_braquet)
			c--;
	}
	if (c != line.end())
	{
		in_config = line.substr(0, i);
		line = line.substr(i);
	}
	if (!nbr_paire_braquet)
	{
		key_Word_Find = false;
		braquet_open = false;
	}
	return (in_config);
}

static void find_start_of_server_config(std::string &line, size_t &nbr_paire_braquet, const std::string key_Word, bool &key_Word_Find, bool &braquet_open)
{
	if (line.compare(0, key_Word.length(), key_Word) == 0)
	{
		std::string	afterKeyWord = line.substr(key_Word.length());
		afterKeyWord.erase(0, afterKeyWord.find_first_not_of(" \t\r\n"));
		afterKeyWord.erase(afterKeyWord.find_last_not_of(" \t\r\n") + 1);
		if ((key_Word_Find || braquet_open) && afterKeyWord.empty())
			throw std::runtime_error("Invalide file.config.\nIl ne peut pas avoir le mot clée server dans ou suivie de server.");
		if (afterKeyWord.empty())
			key_Word_Find = true;
		else if (afterKeyWord[0] == '#')
			return ;
		else if (afterKeyWord[0] == '{')
		{
			line = line.substr(key_Word.length());
			line.erase(0, line.find_first_not_of(" \t\r\n"));
			line.erase(line.find_last_not_of(" \t\r\n") + 1);
			nbr_paire_braquet++;
			key_Word_Find = false;
			braquet_open = true;
		}
		else if (key_Word_Find)
			throw std::runtime_error("Invalide file.config ,\nCe qui doit suivre le mot clée server doit etre une braquet ouvert.");
	}
	else if (key_Word_Find && line.compare(0, 1, "{") == 0)
	{
		nbr_paire_braquet++;
		key_Word_Find = false;
		braquet_open = true;
	}
	else if (key_Word_Find && !(line.empty() || line[0] == '#' ))
		throw std::runtime_error("Invalide file.config .\nCe qui doit suivre le mot clée server doit etre une braquet ouvert.");
}

static std::string clear_comment(std::string line)
{
	if (line.find('#') == std::string::npos)
		return (line);
	return (line.substr(0, line.find('#')));
}

// return un tableau de configuration pour chaque serveur
std::vector<Configuration> getAllConf(std::string file_config)
{
	// 1. Verifier qu'il sagit dun fichier .config ou .conf et qu'on peut l'ouvrire/lire sinon envoyer une exception
	verifFileConfig(file_config);
	std::ifstream file_stream(file_config.c_str());
	if (!file_stream.is_open() || !file_stream.good())
		throw std::runtime_error("Impossible d'ouvrir ou de lire le fichier de configuration: " + file_config);

	std::vector<Configuration> configurations;
	std::string configuration_text = "";
	std::string line;
	const std::string	key_Word = "server";
	bool		key_Word_Find = false;
	bool		braquet_open = false;

	size_t		nbr_paire_braquet = 0;
	
	while (std::getline(file_stream, line))
	{
		line.erase(0, line.find_first_not_of(" \t\r\n"));
		line.erase(line.find_last_not_of(" \t\r\n") + 1);
		line = clear_comment(line);

		if (braquet_open)
		{
			configuration_text = configuration_text + find_end_of_server_config(line, nbr_paire_braquet, key_Word_Find, braquet_open);
			if (!braquet_open)
			{
				std::cout << configuration_text << std::endl  << "---------------------------" << std::endl;// ajouter mtn le type config
				configuration_text = "";
			}
		}
		if (!braquet_open)
			find_start_of_server_config(line, nbr_paire_braquet, key_Word, key_Word_Find, braquet_open);
		if (braquet_open && !line.empty() && line[0] != '#')
			configuration_text = configuration_text + line + "\n";
		if (!braquet_open && !key_Word_Find && !line.empty() && line[0] != '#' && !(line.length() == 1 && line[0] == ';'))
		{
			std::cout << line << std::endl;
			throw std::runtime_error("Invalide file.config\nmot clée or du config serveur.");
		}
	}

	file_stream.close();
	return configurations;
}

