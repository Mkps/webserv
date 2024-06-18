/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 15:30:25 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/18 15:30:26 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Request.hpp"
#include <iostream>
#include <sstream>
Response::Response() {
  _statusCode = 400;
  _body = "<!DOCTYPE "
          "html>\n<html>\n<head>\n</head>\n<body>\n<h3>NO</h3>\n</body>\n</"
          "html>\r\n";
}

Response::Response(Request const &request) { (void)request; }

Response::~Response() {}

Response::Response(Response const &src) { *this = src; }

Response &Response::operator=(Response const &rhs) {
  _statusCode = rhs.getStatusCode();
  for (std::map<std::string, std::string>::const_iterator it =
           rhs._responseHeaders.begin();
       it != rhs._responseHeaders.end(); ++it)
    _responseHeaders[it->first] = it->second;
  return *this;
}

void Response::setStatusCode(short status) { _statusCode = status; }
short const &Response::getStatusCode() const { return _statusCode; }

hashmap const &Response::getHeader() const { return this->_responseHeaders; }

std::string const &Response::getHeaderValue(std::string const &key) const {
  if (_responseHeaders.find(key) == _responseHeaders.end())
    std::cerr << "Value not found" << std::endl;
  return _responseHeaders.find(key)->second;
}

void Response::setBodyError() {

  std::ostringstream s;
  s << "<!DOCTYPE html>\n<html>\n<head>\n</head>\n<body>\n<hl>" << _statusCode
    << "</h1>\n<p>Error</p></body>\n</html>\r\n";
  _body = s.str();
}

std::string Response::getResponseMsg() {
  std::ostringstream s;
  s << "HTTP/1.1 " << _statusCode << " KO\n" << writeHeader() << "\n\n";
  if (_statusCode != 200)
    setBodyError();
  s << _body;
  return s.str();
}

void Response::setHeader(std::string const &key, std::string const &value) {
  _responseHeaders[key] = value;
}

inline std::string getResponse(short status) {
  if (status >= 100 && status < 200) {
    return "Informational";
  } else if (status >= 200 && status < 300) {
    return "Success";
  } else if (status >= 300 && status < 400) {
    return "Redirection";
  } else if (status >= 400 && status < 500) {
    return "Client Error";
  } else if (status >= 500 && status < 600) {
    return "Server Error";
  } else
    return "Bad Request";
}
std::string Response::writeHeader() {
  std::ostringstream s;
  s << "HTTP/1.1 " << _statusCode << " " << getResponse(_statusCode) << "\n";
  if (_statusCode != 200)
    setBodyError();
  for (hashmap::const_iterator it = _responseHeaders.begin();
       it != _responseHeaders.end(); ++it)
    s << it->first << " : " << it->second << ",\n";
  s << "\n";
  return s.str();
}

void Response::send() const {}
