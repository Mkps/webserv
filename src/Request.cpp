/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:09 by aloubier          #+#    #+#             */
/*   Updated: 2024/08/01 18:44:39 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Client.hpp"
#include "http_utils.hpp"
#include <algorithm>
#include <sstream>
#include <stdexcept>

Request::~Request() {}

Request::Request() {}

Request::Request(std::string const &request) : _requestLine(request) {
  fetchData(request);
  _isCGI = false;
  _cgiIndex = -1;
  _cgiPath = "";
  _size = 0;
}

Request::Request(Request const &src) { *this = src; }

Request &Request::operator=(Request const &rhs) {
  _requestLine = rhs.line();
  _requestHeaders = rhs.headers();
  _isCGI = rhs._isCGI;
  _size = rhs._size;
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
  fetchData(request);
}

Request const &Request::getRequest() const { return *this; }

RequestLine const &Request::line() const { return _requestLine; }

std::string const &Request::body() const { return _body; }

hashmap const &Request::headers() const { return _requestHeaders; }

std::string Request::findValue(std::string const &value) const {
  hashmap::const_iterator it = _requestHeaders.find(value);
  if (it == _requestHeaders.end())
    return "";
  return it->second;
}

void Request::fetchData(std::string const &request) {
  size_t pos = request.find("\r\n");
  std::string requestLine = request.substr(0, pos);
  std::string requestData = request.substr(pos + 2);

  pos = requestData.find("\r\n\r\n");
  std::string headers = requestData.substr(0, pos);
  if (pos != _body.npos)
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
      if (pos != value.npos)
        value = value.substr(0, pos);
      std::transform(key.begin(), key.end(), key.begin(), ::tolower);
      _requestHeaders[key] = value;
    }
  }
}

void Request::trimBody() {
  size_t pos = _body.find("\r\n\r\n");
  if (pos != _body.npos)
    _body = _body.substr(0, pos);
}

void Request::unchunkRequest(void) {

  std::string tmp = "";
  std::string::size_type pos = 0;
  std::string::size_type endPos;

  while (pos < _body.length()) {
    endPos = _body.find("\r\n", pos);
    if (endPos == std::string::npos) {
      throw std::runtime_error("Invalid Chunked Transfer Encoding");
      return;
    }
    std::string chunkSizeHex = _body.substr(pos, endPos - pos);

    char *endPtr;
    long chunkSize = std::strtol(chunkSizeHex.c_str(), &endPtr, 16);
    if (*endPtr != '\0' || chunkSize < 0) {
      throw std::runtime_error("Invalid Chunked Transfer Encoding");
      return;
    }
    pos = endPos + 2;

    if (chunkSize == 0) {
      break;
    }

    if (pos + chunkSize > _body.length()) {
      throw std::runtime_error("Invalid Chunked Transfer Encoding");
      return;
    }

    tmp.append(_body, pos, chunkSize);

    pos += chunkSize + 2;
  }
  _body = tmp;
}

inline std::string getExt(std::string const &s) {
  std::string tmp;
  size_t end = s.find_first_of('?');
  tmp = s.substr(0, end);
  size_t start = tmp.find_first_of('.');
  if (start == tmp.npos || start + 1 >= s.size())
    return "";
  return tmp.substr(start + 1);
}

void Request::checkCGI(Client const &client) {
  std::vector<Location> loc = client.getConfig().get_locations();
  for (size_t i = 0; i < loc.size(); ++i) {
    if (loc[i].get_path() == getExt(getFilePath())) {
      _isCGI = true;
      _cgiIndex = i;
      std::string dummy(loc[i].get_value("cgi_path")[0]);
      _cgiPath = dummy;
      return;
    }
  }
  _isCGI = false;
}

std::string Request::getCgiPath() const { return _cgiPath; }

bool Request::isCGI() const { return _isCGI; }

int Request::validateRequest(Client const &cli) const {
  std::string method = _requestLine.getMethod();
  if (!cli.getConfig().is_a_legit_Method(method))
    return logError("Not a legit method", 400);
  if (!_requestLine.isVersionValid())
    return logError("Invalid http version", 400);
  if (_requestLine.getRequestUri().size() > 4096)
    return logError("uri too long", 414);
  if (!_requestLine.isURIValid())
    return logError("Invalid uri", 400);
  if (findValue("host").empty()) {
    return logError("No host set", 400);
  }
  if (findValue("transfer-encoding") != "chunked" &&
      findValue("content-length").empty() && _body.size() > 0)
    return 411;
  if (_body.size() > cli.getConfig().get_client_max_body_size())
    return 413;
  return 0;
}

void Request::clear() {
  _requestHeaders.clear();
  _isCGI = false;
  _body = "";
  _cgiIndex = 0;
  _cgiPath = "";
  _size = 0;
}

void Request::setSize(const size_t &size) { _size = size; }

size_t const &Request::size() const { return _size; }
