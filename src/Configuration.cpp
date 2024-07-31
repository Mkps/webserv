/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 23:53:18 by yzaoui            #+#    #+#             */
/*   Updated: 2024/07/13 16:06:47 by yzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "http_utils.hpp"
#include <sstream>

/////////////////////////////////////////////

Configuration::Configuration() : _id(size_t()) { (void)_id; }

Configuration &Configuration::operator=(const Configuration &src) {
  if (this != &src) {
    this->_locations = src._locations;
    this->_param = src._param;
  }
  return (*this);
}

Configuration::Configuration(const Configuration &src) : _id(src.get_id()) {
  *this = src;
}

Configuration::~Configuration() {}

// 1. Verifier qu'il sagit dun fichier .config ou .conf et qu'on peut
// l'ouvrire/lire sinon envoyer une exception
static void verifFileConfig(std::string file_config) {
  std::string::size_type dot_pos = file_config.find_last_of('.');
  if (dot_pos == std::string::npos)
    throw std::runtime_error("Le fichier n'a pas d'extension: " + file_config);

  std::string extension = file_config.substr(dot_pos);
  if (extension != ".con" && extension != ".config")
    throw std::runtime_error(
        "Le fichier doit avoir une extension .con ou .config: " + file_config);
}

void Configuration::_add_location(std::string str) {
  std::string type_of_location;
  std::string path = "";
  std::string config_location_str;
  size_t start = 0;
  size_t end = 0;

  for (; str[end] != ' ' && str[end] != '\t' && end < str.length() &&
         str[end] != '{';
       end++) {
  }

  if (str[end] == '{')
    throw std::runtime_error(
        "Invalide file.config.\nIl faut un path pour les locations.");
  type_of_location = str.substr(0, end);
  start = end;

  for (; (str[start] == ' ' || str[start] == '\t') && str[start] != '{' &&
         start < str.length();
       start++) {
  }

  if (str[start] == '{')
    throw std::runtime_error(
        "Invalide file.config.\nIl faut un path pour les locations.");
  end = start;

  for (; str[end] != ' ' && str[end] != '\t' && str[end] != '{' &&
         end < str.length();
       end++) {
  }

  path = str.substr(start, end - start);
  for (; (str[end] == ' ' || str[end] == '\t') && str[end] != '{' &&
         end < str.length();
       end++) {
  }
  if (str[end] != '{')
    throw std::runtime_error("Invalide file.config.\nCe qui suit le path d'une "
                             "location est une braquette ouverte.");
  config_location_str = str.substr(end);
  this->_locations.push_back(
      Location(type_of_location, path, config_location_str));
}

/// @brief Constructeur parametric de Configuration
/// @param config_str une chaine de caractere contant tout ce qu'il y a entre
/// les braquet ouvert et fermé
/// @param id du serveur, qui est attitré
Configuration::Configuration(std::string config_str, size_t id) : _id(id) {
  std::string param;
  size_t i = 1;
  size_t start = i;
  bool inside_braquet = false;
  // std::cout << YELLOW + config_str + NOCOLOR << std::endl << std::endl;

  std::string::iterator no_newline =
      std::remove(config_str.begin(), config_str.end(), '\n');

  config_str.erase(no_newline, config_str.end());
  // config_str.replace(config_str.begin(), config_str.end(), '\n', ' ');
  // std::cout << GREEN + config_str + NOCOLOR << std::endl << std::endl;

  for (; i < config_str.length() - 1; i++) {
    if (config_str[i] == '{')
      inside_braquet = true;
    if (!inside_braquet && config_str[i] == ';') {
      add_key_value(config_str.substr(start, i - start), this->_param);
      start = i + 1;
    } else if (inside_braquet && config_str[i] == '}') {
      inside_braquet = false;
      this->_add_location(config_str.substr(start, i - start + 1));
      start = i + 1;
    }
  }
}

size_t Configuration::get_id(void) const { return (this->_id); }

std::vector<std::string> Configuration::get_value(std::string key) const {
  std::map<std::string, std::vector<std::string> >::const_iterator it =
      this->_param.find(key);

  if (it != _param.end())
    return it->second;
  else
    return std::vector<std::string>();
}

std::vector<std::string> Configuration::get_all_key(void) {
  std::vector<std::string> all_key;

  for (std::map<std::string, std::vector<std::string> >::iterator it =
           this->_param.begin();
       it != this->_param.end(); ++it)
    all_key.push_back(it->first);
  return (all_key);
}

std::vector<Location> Configuration::get_locations(void) const {
  return (this->_locations);
}

std::vector<Location>
Configuration::get_locations_by_path(std::string rpath) const {
    std::string path(rpath);
  size_t pos = path.find_last_of("/");
  if (*path.rbegin() != '/') {
      path = path.substr(0, pos + 1);
  }
  std::vector<Location>::const_iterator it = this->_locations.begin();
  std::vector<Location> res = std::vector<Location>();
  for (; it != this->_locations.end(); ++it) {
    if (it->get_path() == path){
      res.push_back(*it);
    }
  }
  return res;
}

void Configuration::show(void) {
  std::cout << "id of configuration : " << this->get_id() << std::endl
            << "{" << std::endl;

  std::vector<std::string> all_key = this->get_all_key();
  for (std::vector<Location>::iterator it_loc = this->_locations.begin();
       it_loc != this->_locations.end(); it_loc++)
    (*it_loc).show();
  for (std::vector<std::string>::const_iterator it_key = all_key.begin();
       it_key != all_key.end(); it_key++) {
    std::vector<std::string> values = this->get_value(*it_key);
    std::cout << RED << *it_key << NOCOLOR " = ";
    for (std::vector<std::string>::const_iterator it_value = values.begin();
         it_value != values.end(); it_value++)
      std::cout << "\"" << BLUE << *it_value << NOCOLOR "\" ";
    std::cout << ";" << std::endl;
  }
  std::cout << "}" << std::endl;
}

// methode qui retourne la valeur de la taille maximum du client
// en unité de mesure M qui signifie megabyte
//  si il na pas la configuration ou erreur envoie 0
size_t Configuration::get_client_max_body_size(void) const {
  std::vector<std::string> all_res = this->get_value("client_max_body_size");
  if (all_res.empty())
    return (0);
  // rien ou B pour Byte
  // K = pour kilo *10**3
  // M = pour Mega * 10**6
  std::string str_value = all_res[0];
  size_t maxbodysize = 0;
  size_t puissance = 1;
  std::string::const_iterator i = str_value.begin();
  for (; i != str_value.end() && ((*i) >= '0' && (*i) <= '9'); i++) {
    maxbodysize = maxbodysize * 10 + ((*i) - '0');
  }
  if (i != str_value.end()) {
    if (*i == 'K')
      puissance = 10 * 10 * 10;
    else if (*i == 'M')
      puissance = 10 * 10 * 10 * 10 * 10 * 10;
    else if (*i != 'B')
      return (0);
    i++;
    if (i != str_value.end())
      return (0);
  }
  return (maxbodysize * puissance);
}

std::vector<std::string> split(const std::string &s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);

  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

// verifie si une une methode GET POST ou DELETE Est allowed sinon return False
bool Configuration::is_a_allowed_Method(const std::string &methode,
                                        std::string const &path) const {
  std::vector<std::string> all_res;
  std::vector<Location> l = get_locations_by_path(path);
  if (!l.empty())
    all_res = l[0].get_value("allowedMethods");
  if (all_res.empty())
    all_res = this->get_value("allowedMethods");
  if (all_res.empty())
    return (false);
  std::string str_value = all_res[0];
  std::vector<std::string> allMEthod = split(str_value, '|');
  for (std::vector<std::string>::iterator i = allMEthod.begin();
       i != allMEthod.end(); i++) {
    (*i).erase(0, (*i).find_first_not_of(" \t\r\n"));
    (*i).erase((*i).find_last_not_of(" \t\r\n") + 1);
    if (*i == methode)
      return (true);
  }
  return (false);
}

// retourne le chemin d'upload si vide ou rien alors retourne ""
std::string Configuration::get_path_upload(void) const {
  std::vector<std::string> all_res = this->get_value("upload_path");
  if (all_res.empty() || all_res[0].empty())
    return ("");
  return (all_res[0]);
}

// retourne une redirection
std::string Configuration::get_redirect(std::string const &path) const {
  std::vector<std::string> all_res;
  std::vector<Location> l = get_locations_by_path(path);
  std::string tmpstr = "";
  if (l.empty())
      return "";
  all_res = l[0].get_value("redirect");
  if (all_res.empty() || all_res[0].empty())
    return ("");
  return all_res[0];
}

// retourne la page derreur en fonction du code et retourne une string vide si
// il na pas etais parametre
std::string Configuration::get_error_page(size_t error_code) const {
  std::vector<std::string> all_res = this->get_value("error_page");
  if (all_res.empty())
    return ("");
  std::ostringstream tmp;
  tmp << error_code;
  std::string error_code_str = tmp.str();
  for (std::vector<std::string>::const_iterator it = all_res.begin();
       it != all_res.end(); it++) {
    std::string code = (*it).substr(0, 3);
    std::string res = (*it).substr(3);
    res.erase(0, res.find_first_not_of(" \t\r\n"));
    res.erase(res.find_last_not_of(" \t\r\n") + 1);
    if (code == error_code_str)
      return (res);
  }
  return ("");
}

// verifie si il sagit dune methode legit
bool Configuration::is_a_legit_Method(const std::string &method_name) const {
  return (method_name == "POST" || method_name == "GET" ||
          method_name == "DELETE");
}

////////////////////////////////////////////////////////////////////////

static std::string find_end_of_server_config(std::string &line,
                                             size_t &nbr_paire_braquet,
                                             bool &key_Word_Find,
                                             bool &braquet_open) {
  std::string in_config = std::string();
  size_t i = 0;
  std::string::const_iterator c = line.begin();
  for (; nbr_paire_braquet && c != line.end(); c++, i++) {
    if (*c == '{')
      nbr_paire_braquet++;
    else if (*c == '}')
      nbr_paire_braquet--;
    if (!nbr_paire_braquet)
      c--;
  }
  if (c != line.end()) {
    in_config = line.substr(0, i);
    line = line.substr(i);
  }
  if (!nbr_paire_braquet) {
    key_Word_Find = false;
    braquet_open = false;
  }
  return (in_config);
}

static void find_start_of_server_config(std::string &line,
                                        size_t &nbr_paire_braquet,
                                        const std::string key_Word,
                                        bool &key_Word_Find,
                                        bool &braquet_open) {
  if (line.compare(0, key_Word.length(), key_Word) == 0) {
    std::string afterKeyWord = line.substr(key_Word.length());
    afterKeyWord.erase(0, afterKeyWord.find_first_not_of(" \t\r\n"));
    afterKeyWord.erase(afterKeyWord.find_last_not_of(" \t\r\n") + 1);
    if ((key_Word_Find || braquet_open) && afterKeyWord.empty())
      throw std::runtime_error("Invalide file.config.\nIl ne peut pas avoir le "
                               "mot clée server dans ou suivie de server.");
    if (afterKeyWord.empty())
      key_Word_Find = true;
    else if (afterKeyWord[0] == '#')
      return;
    else if (afterKeyWord[0] == '{') {
      line = line.substr(key_Word.length());
      line.erase(0, line.find_first_not_of(" \t\r\n"));
      line.erase(line.find_last_not_of(" \t\r\n") + 1);
      nbr_paire_braquet++;
      key_Word_Find = false;
      braquet_open = true;
    } else if (key_Word_Find)
      throw std::runtime_error("Invalide file.config ,\nCe qui doit suivre le "
                               "mot clée server doit etre une braquet ouvert.");
  } else if (key_Word_Find && line.compare(0, 1, "{") == 0) {
    nbr_paire_braquet++;
    key_Word_Find = false;
    braquet_open = true;
  } else if (key_Word_Find && !(line.empty() || line[0] == '#'))
    throw std::runtime_error("Invalide file.config .\nCe qui doit suivre le "
                             "mot clée server doit etre une braquet ouvert.");
}

static std::string clear_comment(std::string line) {
  if (line.find('#') == std::string::npos)
    return (line);
  return (line.substr(0, line.find('#')));
}

// return un tableau de configuration pour chaque serveur
std::vector<Configuration> getAllConf(std::string file_config) {
  verifFileConfig(file_config);
  std::ifstream file_stream(file_config.c_str());
  if (!file_stream.is_open() || !file_stream.good())
    throw std::runtime_error(
        "Impossible d'ouvrir ou de lire le fichier de configuration: " +
        file_config);

  std::vector<Configuration> configurations;
  std::string configuration_text = "";
  std::string line;
  const std::string key_Word = "server";
  bool key_Word_Find = false;
  bool braquet_open = false;

  size_t nbr_paire_braquet = 0;

  while (std::getline(file_stream, line)) {
    line.erase(0, line.find_first_not_of(" \t\r\n"));
    line.erase(line.find_last_not_of(" \t\r\n") + 1);
    line = clear_comment(line);

    if (braquet_open) {
      configuration_text =
          configuration_text +
          find_end_of_server_config(line, nbr_paire_braquet, key_Word_Find,
                                    braquet_open);
      if (!braquet_open) {
        configurations.push_back(
            Configuration(configuration_text, configurations.size() + 1));
        configuration_text = "";
      }
    }
    if (!braquet_open)
      find_start_of_server_config(line, nbr_paire_braquet, key_Word,
                                  key_Word_Find, braquet_open);
    if (braquet_open && !line.empty() && line[0] != '#')
      configuration_text = configuration_text + line + "\n";
    if (!braquet_open && !key_Word_Find && !line.empty() && line[0] != '#' &&
        !(line.length() == 1 && line[0] == ';')) {
      std::cout << line << std::endl;
      throw std::runtime_error(
          "Invalide file.config\nmot clée hors du config serveur.");
    }
  }

  file_stream.close();
  if (nbr_paire_braquet)
    throw std::runtime_error(
        "Invalide file.config, une Braquette n'a pas etais refermée.");
  return configurations;
}
