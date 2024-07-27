/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:05 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/17 14:51:06 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "RequestLine.hpp"
#include <map>

typedef std::map<std::string, std::string> hashmap;
class Client;
class Request {
private:
  RequestLine _requestLine;
  hashmap _requestHeaders;
  std::string _body;
  bool _isCGI;
  int _cgiIndex;
  std::string _cgiPath;
  void retrieveHeaders(std::string const &request);
  void fetchData(std::string const &request);
  int initData(); // will eventually be taken over by Server->conf

public:
  Request();
  Request(std::string const &request);
  ~Request();
  Request(Request const &src);
  Request &operator=(Request const &rhs);

  int validateRequest(Client const &cli) const;
  std::string getFilePath() const;
  std::string getAbsPath() const;
  std::string getCgiPath() const;
  void setRequest(std::string const &request);
  Request const &getRequest() const;
  void checkCGI(Client const &client);
  bool isCGI() const;
  RequestLine const &line() const;
  hashmap const &headers() const;
  std::string const &body() const;
  std::string findValue(std::string const &value) const;
};
#endif
