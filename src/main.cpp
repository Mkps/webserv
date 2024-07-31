/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaoui <yzaoui@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/14 16:28:38 by aloubier          #+#    #+#             */
/*   Updated: 2024/07/28 18:17:39 by yzaoui           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "Server.hpp"

#include <iostream>
#include <stdexcept>

// Function to check if a path is a directory
bool isDirectory(const std::string& path) {
  struct stat info;
  if (stat(path.c_str(), &info) != 0) {
    return false;
  }
  return (info.st_mode & S_IFDIR) != 0;
}

// Function to check if a path is a file
bool isFile(const std::string& path) {
  struct stat info;
  if (stat(path.c_str(), &info) != 0) {
    return false;
  }
  return (info.st_mode & S_IFREG) != 0;
}
int main(int ac, char **av)
{
  const std::string defaultfile = "configurations/minimal.config";
  //try {
    std::string fileConfig = defaultfile;
    if (ac > 2 || ac < 2)
      throw std::invalid_argument("Incorrect argument count.");
    else
    {
      if (isFile(av[1]))
        fileConfig = av[1];
      else if (!isDirectory(av[1]))
        throw std::invalid_argument("Incorrect argument it's not a Directory and not a File.");
    }
    std::vector<Configuration> ConfigurationForAllServ = getAllConf(fileConfig);
    for (std::vector<Configuration>::iterator it =
             ConfigurationForAllServ.begin();
         it != ConfigurationForAllServ.end(); it++) {
      (*it).show();
      std::cout << "----------" << std::endl;
    }
    Server *server = new Server(ConfigurationForAllServ);
    server->run();
    delete server;
  /*} catch (const std::exception &e) {*/
  /*  std::cerr << RED "Error: " NOCOLOR << e.what() << '\n';*/
  /*  return (1);*/
  /*}*/
  return 0;
}
