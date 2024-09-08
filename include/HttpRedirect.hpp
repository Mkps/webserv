/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRedirect.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 18:45:44 by aloubier          #+#    #+#             */
/*   Updated: 2024/08/01 18:45:44 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Configuration.hpp"
#include <string>
class Response;
class Request;
class Configuration;
// Used as an interface between the request and the response allowing to define
// the location
class HttpRedirect {
public:
  HttpRedirect();
  ~HttpRedirect();
  HttpRedirect(HttpRedirect const &src);
  HttpRedirect &operator=(HttpRedirect const &rhs);
  static void handleRedirect(Request const &req, Response &response, Configuration const &conf);
  static int handleFolder(Request const &req, Response &response, Configuration const &conf);
  bool isRedirect();
  std::string const &location();

private:
  std::string _location;
  bool _redirect;
};
