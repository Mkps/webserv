/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 15:30:25 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/19 17:49:57 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Request.hpp"
#include "http_utils.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <vector>

Response::Response() {
  // If this is used something went very wrong...
  _statusCode = 500;
  _body = "<!DOCTYPE "
          "html>\n<html>\n<head>\n</head>\n<body>\n<h1>500</h1>\n<p>Internal "
          "Server Error</p></body>\n</"
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

void Response::setHeader(std::string const &key, std::string const &value) {
  _responseHeaders[key] = value;
}

inline std::string getResponse(short status) {
  if (status >= 100 && status < 200) {
    return "Informational";
  } else if (status >= 200 && status < 300) {
    if (status == 204)
      return "No Response";
    return "Success";
  } else if (status >= 300 && status < 400) {
    return "Redirection";
  } else if (status >= 400 && status < 500) {
    if (status == 403)
      return "Forbidden";
    else if (status == 404)
      return "File Not Found";
    else
      return "Client Error";
  } else if (status >= 500 && status < 600) {
    return "Server Error";
  } else
    return "Bad Request";
}

void Response::processRequest(Request const &req) {
  if (req.validateRequest() < 0) {
    setBodyError(400);
    return;
  }
  std::string root = "./resources";
  std::string path = root + req.getFilePath();
  _path = path;
  setStatusCode(200);
  if (req.getRequestLine().getMethod() == "GET") {
    std::cout << "GET detected" << std::endl;
    httpMethodGet(req);
  } else if (req.getRequestLine().getMethod() == "POST") {
    std::cout << "POST detected" << std::endl;
    httpMethodPost(req);
  } else if (req.getRequestLine().getMethod() == "DELETE") {
    std::cout << "DELETE detected" << std::endl;
    httpMethodDelete(req);
  } else {
    std::cout << "valid method but not handled?" << std::endl;
	setStatusCode(400);
    setHeader("content-length", "");
    setHeader("content-type", "text/plain");
    setBodyError(_statusCode);
  }
}

void Response::setBodyError(int status) {

  std::ostringstream s;
  _statusCode = status;
  setHeader("content-length", "");
  setHeader("content-type", "text/plain");
  s << "<!DOCTYPE html>\n<html>\n<head>\n</head>\n<body>\n<hl>" << _statusCode
    << "</h1>\n<p>" << getResponse(_statusCode) << "</p></body>\n</html>\r\n";
  _body = s.str();
}

std::string Response::getResponseMsg() {
  std::ostringstream s;
  s << "HTTP/1.1 " << _statusCode << " KO\n" << writeHeader() << "\n\n";
  if (_statusCode != 200)
    setBodyError(400);
  s << _body;
  return s.str();
}

void Response::setBody(std::string const &filename) {

  std::ifstream file(filename.c_str());
  std::ostringstream s;
  std::string line;
  while (getline(file, line)) {
    s << line;
  }
  _body = s.str();
  file.close();
}

std::string Response::writeHeader() {
  std::ostringstream s;
  s << "HTTP/1.1 " << _statusCode << " " << getResponse(_statusCode) << "\n";
  for (hashmap::const_iterator it = _responseHeaders.begin();
       it != _responseHeaders.end(); ++it)
    s << it->first << " : " << it->second << ",\n";
  s << "\r\n";
  std::cout << s.str() << std::endl;
  return s.str();
}

std::string			Response::findContentType(){
	size_t pos;
	pos = _path.find_last_of(".");
	std::string type = _path.substr(pos + 1);
	if (type == "html")
		return "text/html";
	else if (type == ".css")
		return "text/css";
	else if (type == "js")
		return "text/javascript";
	else if (type == "jpeg" || type == "jpg")
		return "image/jpeg";
	else if (type == "png")
		return "image/png";
	else if (type == "bmp")
		return "image/bmp";
	return "text/plain";
}

inline void sendStr(int clientSocket, std::string const &str) {
  send(clientSocket, str.c_str(), strlen(str.c_str()), 0);
}
void Response::sendResponse(int clientSocket) {
  sendStr(clientSocket, writeHeader());
  sendStr(clientSocket, _body);
}

void Response::httpMethodDelete(Request const &req) {
  std::string root = "./resources";
  std::string path = root + req.getFilePath();
  if (fileStatus(path) == FILE_REG || fileStatus(path) == FILE_DIR) {
    if (remove(path.c_str()) == EXIT_SUCCESS)
      _statusCode = 204;
    else
      _statusCode = 403;
  } else
    _statusCode = 404;
  if (_statusCode >= 400)
    setBodyError(_statusCode);
  else
    setBody("");
}
void Response::httpMethodGet(Request const &req) {
  (void)req; // req will be needed for the cgi(env + headers)
  std::string myStr[] = {"index", "index.html"};
  std::vector<std::string> tryFiles;
  std::string root = "./resources";
  std::string path = root + req.getFilePath();
  tryFiles.assign(myStr, myStr + 2);
  if (fileStatus(path) == FILE_DIR) {
    for (std::vector<std::string>::const_iterator it = tryFiles.begin();
         it != tryFiles.end(); ++it) {
      std::string filePath(path + *it);
      if (fileStatus(filePath) == FILE_NOT) {
        setBodyError(404);
      } else
        break;
    }
  } else if (fileStatus(path) == FILE_NOT) {
    setBodyError(404);
  }
  if (_statusCode == 200)
    _path = path;
  bool isCGI = 0;
  if (isCGI) {
    std::cout << "Do cgi stuff here" << std::endl;
  } else if (_statusCode == 200) { // We have a valid file
	std::ostringstream ss;
	ss << _body.size();
    setHeader("Content-Length", "");
    setHeader("Content-Type", findContentType());
    setBody(_path);
	std::cout << "path " << _path << " size " << ss.str() << " type " << findContentType() <<std::endl;
  } else {
    setBodyError(_statusCode);
  }
}
inline std::string generate_filename() {
    std::ostringstream oss;
    time_t t = time(NULL);
    oss << "resource_" << t;
    return oss.str();
}

void Response::httpMethodPost(Request const &req) {
  (void)req; // will probably need it... Maybe
  std::string root = "./resources";
  std::string path = root + req.getFilePath();
  bool isCGI = 0;
  if (isCGI) {
    std::cout << "Do cgi stuff here" << std::endl;
  } else if (_statusCode == 200) {
    std::cout << "If not cgi but post" << std::endl;
    std::ofstream outFile;
	if (fileStatus(path) == FILE_DIR)
		path += generate_filename();
    outFile.open(path.c_str(), std::ios::out | std::ios::binary | std::ios::ate | std::ios::app);
    if (!outFile.good()) {
      _statusCode = 500;
	  std::cout << "badfile" << std::endl;
      return;
    }
	outFile.write(req.getRequestBody().c_str(), req.getRequestBody().size());
	std::cout << "goodfile " << _statusCode << std::endl;
	_statusCode = 204;
	return ;
  } else {
    setBodyError(_statusCode);
  }
}
