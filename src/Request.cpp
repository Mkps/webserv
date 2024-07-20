/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:09 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/17 14:51:09 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Client.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

Request::~Request() {}

Request::Request() {}

Request::Request(std::string const &request) : _requestLine(request) {
  if (_requestLine.isRequestLineValid() < 0)
    std::cerr << "invalid method or http version" << std::endl;
  fetchData(request);
}

Request::Request(Request const &src) { *this = src; }

Request &Request::operator=(Request const &rhs) {
  _requestLine = rhs.line();
  _requestHeaders = rhs.headers();
  return *this;
}

// Will eventually need to take into account the host provided by config
std::string Request::getFilePath() const {
  std::ostringstream s;
  std::string uri = _requestLine.getRequestUri();
  size_t pos = uri.find("\r\n");
  uri.substr(0, pos);
  s << _requestLine.getRequestUri();
  return s.str();
}

std::string Request::getAbsPath() const {
  std::ostringstream ss;
  std::string host = _requestHeaders.find("host")->second;

  ss << "http://" << host << _requestLine.getRequestUri();
  return ss.str();
}

void Request::setRequest(std::string const &request) {
  RequestLine req(request);
  _requestLine = req;
  if (_requestLine.isRequestLineValid() < 0)
    std::cerr << "invalid method or http version" << std::endl;
  fetchData(request);
}

Request const &Request::getRequest() const { return *this; }

RequestLine const &Request::line() const { return _requestLine; }
std::string const &Request::body() const { return _body; }

hashmap const &Request::headers() const { return _requestHeaders; }

inline void trim(std::string &s) {
  size_t first = s.find_first_not_of(' ');
  size_t last = s.find_last_not_of(' ');
  s = s.substr(first, last - first + 1);
}

// trim from both ends (copying)
inline std::string trim_copy(std::string s) {
  trim(s);
  return s;
}

void Request::fetchData(std::string const &request) {
  size_t pos = request.find("\r\n");
  std::string requestLine = request.substr(0, pos);
  std::string requestData = request.substr(pos + 2);

  pos = requestData.find("\r\n\r\n");
  std::string headers = requestData.substr(0, pos);
  _body = requestData.substr(pos + 4);
  std::istringstream header_stream(headers);
  std::string header;
  size_t index;
  while (std::getline(header_stream, header)) {
    index = header.find(": ", 0);
    if (index != std::string::npos) {
      std::string key = trim_copy(header.substr(0, index));
      std::string value = trim_copy(header.substr(index + 1));
      pos = value.find("\r");
      value = value.substr(0, pos);
      std::transform(key.begin(), key.end(), key.begin(), ::tolower);
      _requestHeaders[key] = value;
    }
  }
}

bool Request::isCGI() const {
  if (_requestLine.getRequestUri().find("/cgi-bin/") == 0)
    return true;
  return false;
}

int Request::validateRequest(Client const &cli) const { 
    if (cli.getConfig().get_client_max_body_size() > _body.size())
        return 413;
    return 0; }
