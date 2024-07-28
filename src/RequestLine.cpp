#include "RequestLine.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

RequestLine::RequestLine() {
  _method = "";
  _requestUri = "";
  _httpVersion = "";
}

RequestLine::RequestLine(std::string const &requestLine) {
  int start = 0;
  int i = 0;
  std::istringstream iss(requestLine);
  while (iss.str()[i] && !std::isspace(iss.str()[i]))
    i++;
  _method = iss.str().substr(start, i);
  while (iss.str()[i] && std::isspace(iss.str()[i]))
    i++;
  start = i;
  while (iss.str()[i] && !std::isspace(iss.str()[i]))
    i++;
  _requestUri = iss.str().substr(start, i - start);
  while (iss.str()[i] && std::isspace(iss.str()[i]))
    i++;
  start = i;
  while (iss.str()[i] && iss.str()[i] != '\r' && iss.str()[i + 1] &&
         iss.str()[i + 1] != '\n')
    i++;
  _httpVersion = iss.str().substr(start, i - start);
}

RequestLine::~RequestLine() {}

RequestLine::RequestLine(RequestLine const &src) { *this = src; }

RequestLine &RequestLine::operator=(RequestLine const &rhs) {
  _method = rhs._method;
  _requestUri = rhs._requestUri;
  _httpVersion = rhs._httpVersion;
  return *this;
}

std::string const &RequestLine::getMethod() const { return _method; }

void RequestLine::setMethod(std::string &method) { _method = method; }

std::string const &RequestLine::getRequestUri() const { return _requestUri; }

void RequestLine::setRequestUri(std::string &Uri) { _requestUri = Uri; }

std::string const &RequestLine::getHttpVersion() const { return _httpVersion; }

void RequestLine::setHttpVersion(std::string &ver) { _httpVersion = ver; }

bool RequestLine::isVersionValid() const {
  if (_httpVersion != "HTTP/1.0" && _httpVersion != "HTTP/1.1")
    return false;
  return true;
}

inline bool validUriChar(char c) {
  if (isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~' || c == '/')
    return true;
  return false;
}
bool RequestLine::isURIValid() const {
  std::cout << "_rUri " << _requestUri << std::endl;
  for (size_t i = 0; i < _requestUri.size(); ++i) {
      if (!validUriChar(_requestUri[i]))
              return false;
  }
  return true;
}

bool RequestLine::isRLValid() const {
	std::vector<std::string> validMethod;
	validMethod.push_back("GET");
	validMethod.push_back("POST");
	validMethod.push_back("DELETE");
	validMethod.push_back("PUT");
	validMethod.push_back("HEAD");
	validMethod.push_back("TRACE");
	validMethod.push_back("OPTIONS");
	if (std::find(validMethod.begin(), validMethod.end(), _method) == validMethod.end())
		return -1;
	if (_httpVersion != "HTTP/1.0" && _httpVersion != "HTTP/1.1")
		return -2;
	return 0;
}
