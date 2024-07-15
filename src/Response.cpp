/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obouhlel <obouhlel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 15:30:25 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/25 11:42:06 by obouhlel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "CgiHandler.hpp"
#include "Cookie.hpp"
#include "HttpAutoindex.hpp"
#include "Request.hpp"
#include "http_utils.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include "HttpRedirect.hpp"

Response::Response() { setDefaultHeaders(); }

Response::Response(Request const &request) {
  (void)request;
  setDefaultHeaders();
}

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

std::string Response::getHeaderValue(std::string const &key) const {
  if (_responseHeaders.find(key) == _responseHeaders.end())
    return std::string();
  return _responseHeaders.find(key)->second;
}

void Response::setHeader(std::string const &key, std::string const &value,
                         bool overwrite) {
  hashmap::iterator it = _responseHeaders.find(key);
  if (it != _responseHeaders.end() && !overwrite) {
    return;
  }
  _responseHeaders[key] = value;
}

inline std::string getResponse(short status) {
  if (status >= 100 && status < 200) {
    return "Informational";
  } else if (status >= 200 && status < 300) {
    if (status == 204)
      return "No Response";
    return "OK";
  } else if (status >= 300 && status < 400) {
    return "Redirection";
  } else if (status >= 400 && status < 500) {
    if (status == 403)
      return "Forbidden";
    else if (status == 404)
      return "File Not Found";
    else if (status == 405)
      return "Method Not Allowed";
    else
      return "Client Error";
  } else if (status >= 500 && status < 600) {
    if (status == 502)
      return "Bad Gateway";
    return "Server Error";
  } else
    return "Bad Request";
}

void Response::processRequest(Request const &req) {
  if (req.validateRequest() < 0) {
    setBodyError(400);
    return;
  }
  _statusCode = 200;
  HttpRedirect::handleRedirect(req, *this);
  std::string s = "No host set";
  if (req.headers().find("host") != req.headers().end())
      s = req.headers().find("host")->second;
  std::cout << s << std::endl;

  if (_statusCode == 403 && true) { // if no substitution were found and the autoindex is on
    _statusCode = 200;
    _body = HttpAutoindex::generateIndex(req,_path);
    setHeader("Content-Length", sizeToStr(_body.size()), true);
    setHeader("Content-Type", "text/html", true);
    return ;
  }
  if (req.line().getMethod() == "GET") {
    httpMethodGet(req);
  } else if (req.line().getMethod() == "POST") {
    httpMethodPost(req);
  } else if (req.line().getMethod() == "DELETE") {
    httpMethodDelete(req);
  } else {
    _statusCode = 405;
  }
  if (_statusCode >= 200 && _statusCode < 300) {
    setHeader("Content-Length", sizeToStr(_body.size()), true);
    setHeader("Content-Type", findContentType(), true);
  } else {
    setBodyError(_statusCode);
  }
}

void Response::setBodyError(int status) {
  std::ostringstream s;
  _statusCode = status;
  s << "<!DOCTYPE html>\n<html>\n<head>\n</head>\n<body>\n<hl>" << _statusCode
    << "</h1>\n<p>" << getResponse(_statusCode) << "</p></body>\n</html>\r\n";
  _body = s.str();
  setHeader("Content-Length", sizeToStr(_body.size()), true);
  setHeader("Content-Type", "text/html", true);
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
  std::ifstream file(filename.c_str(),
                     std::ios::in | std::ios::binary | std::ios::ate);
  std::ifstream::pos_type file_size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::string file_buffer;
  file_buffer.resize(file_size);
  file.read(&file_buffer[0], file_size);
  _body = file_buffer;
  file.close();
}

std::string Response::writeHeader() {
  std::ostringstream s;
  s << "HTTP/1.1 " << _statusCode << " " << getResponse(_statusCode) << "\r\n";
  for (hashmap::const_iterator it = _responseHeaders.begin();
       it != _responseHeaders.end(); ++it)
    s << it->first << ": " << it->second << "\r\n";
  s << "\r\n";
  return s.str();
}

std::string Response::findContentType() {
  size_t pos;
  pos = _path.find_last_of(".");
  std::string type = _path.substr(pos + 1);
  if (type == "html")
    return "text/html";
  else if (type == "css")
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

void Response::setDefaultHeaders() {
  setHeader("Date", get_current_date(), true);
  setHeader("Content-Length", "", true);
  setHeader("Content-Type", "text/plain", true);
  setHeader("Connection", "keep-alive", true);
  setHeader("Charset", "UTF-8", true);
  setHeader("Server", "webserv/0.1", true);
}

void Response::clear() {
  _responseHeaders.clear();
  _statusCode = 200;
  setDefaultHeaders();
}
std::string Response::chunkResponse() {
  const size_t chunk_size = 1024;
  std::string chunked_body;

  if (_offset < _body.size()) {
    size_t len = std::min(chunk_size, _body.size() - _offset);
    std::string chunk = _body.substr(_offset, len);
    _offset += len;
    return chunk;
  } else {
    chunked_body = "0\r\n\r\n";
  }
  return chunked_body;
}
void Response::sendResponse(int clientSocket) {
  std::ostringstream res;
  _offset = 0;
  _bytes_sent = 0;
  if (_body.size() > 1024) {
    setHeader("Transfer-Encoding", "chunked", true);
    setHeader("Connection", "keep-alive", true);
    res << writeHeader();
    sendStr(clientSocket, res.str());
    res.clear();
    while (_offset < _body.size()) {
      std::string chunk = chunkResponse();
      _bytes_sent += sendChunk(clientSocket, chunk);
    }
    std::string chunk = chunkResponse();
    sendStr(clientSocket, chunk);
  } else {
    std::cout << "body is " << _body << std::endl;
    res << writeHeader() << _body << "\r\n\r\n";
    if (getHeaderValue("Content-Type") == "text/html")
      std::cout << writeHeader() << std::endl;
    sendStr(clientSocket, res.str());
  }
}

void Response::httpMethodDelete(Request const &req) {
  (void)req;
  if (remove(_path.c_str()) == EXIT_SUCCESS)
    _statusCode = 204;
  else
    _statusCode = 403;
}

inline std::string cutQuery(std::string const &path) {

  size_t query_pos = path.find("?");
  if (query_pos != std::string::npos) {
    return path.substr(0, query_pos);
  } else
    return path;
}
void Response::findPath(Request const &req) {
  std::string myStr[] = {"index", "index.html"};
  std::string root = "./resources";
  std::string path = root + req.getFilePath();

  _path = path;
  if (req.isCGI())
    return;
  // Check for redirection here
  std::vector<std::string> tryFiles;
  tryFiles.assign(myStr, myStr + 2);
  if (fileStatus(path) == FILE_REG) {
    _path = std::string(path);
    return;
  }
  if (fileStatus(path) == FILE_DIR) {
    for (std::vector<std::string>::const_iterator it = tryFiles.begin();
         it != tryFiles.end(); ++it) {
      std::string filePath(path + *it);
      if (fileStatus(filePath) == FILE_REG) {
        _path = std::string(filePath);
        return;
      } else if (fileStatus(filePath) != FILE_NOT) {
        _statusCode = 403;
      }
    }
  } else if (fileStatus(path) == FILE_NOT) {
    _statusCode = 404;
  }
}

void Response::httpMethodGet(Request const &req) {
  if (_statusCode == 200 && req.isCGI()) {
    CgiHandler cgi(_path);
    _statusCode = cgi.handleGet();
    if (_statusCode == 200)
      _body = cgi.body();
  }
  if (!req.isCGI() && _statusCode == 200) {
    if (!access(_path.c_str(), F_OK | R_OK))
      setBody(_path);
    else
      _statusCode = 403;
  }
}

inline std::string generate_filename() {
  std::ostringstream oss;
  time_t t = time(NULL);
  oss << "resource_" << t;
  return oss.str();
}

void Response::httpMethodPost(Request const &req) {
  if (req.isCGI()) {
    CgiHandler cgi(_path);
    cgi.setRequestBody(req.body());
    int ret = cgi.handlePost();
    if (ret == 200)
      _body = cgi.body();
  } else if (_statusCode == 200) {
    std::ofstream outFile;
    if (fileStatus(_path) == FILE_DIR)
      _path += generate_filename();
    if (access(_path.c_str(), F_OK | W_OK)) {
      _statusCode = 403;
      return;
    }
    outFile.open(_path.c_str(), std::ios::out | std::ios::binary |
                                    std::ios::ate | std::ios::app);
    if (!outFile.good()) {
      _statusCode = 500;
      return;
    }
    outFile.write(req.body().c_str(), req.body().size());
    _statusCode = 204;
    return;
  }
}
