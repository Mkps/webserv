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
#include "Client.hpp"
#include "Configuration.hpp"
#include "Cookie.hpp"
#include "HttpAutoindex.hpp"
#include "HttpMethod.hpp"
#include "HttpRedirect.hpp"
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


void Response::processCgi(Request &req, Client &client) {
  (void)req;
  std::cout << "processCgi" << std::endl;
  if (_cgi.isRunning()) {
    if (_cgi.getProcessState() > 0) {
      _statusCode = _cgi.getStatus();
      if (_statusCode == 200) {
        _body = _cgi.body();
        if (_body.find("Content-Length") == _body.npos)
          setHeader("Content-Length", sizeToStr(_body.size()), true);
        if (_body.find("Content-Type") == _body.npos)
          setHeader("Content-Type", "text/plain", true);
      } else {
        setBodyError(_statusCode, errPage(client, _statusCode));
      }
      client.setState(C_RES);
    } else {
      if (_cgi.timeout()) {
        std::cout << "CGI TIMEOUT" << std::endl;
        _cgi.killCgi();
        _statusCode = 504;
        setBodyError(_statusCode, errPage(client, _statusCode));
        client.setState(C_RES);
      }
    }
  }
}

bool Response::isAutoindex(Request const &req, Configuration const &conf) {
    if (_statusCode != 403)
        return false;
    std::vector<Location> l = conf.get_locations_by_path(req.getFilePath());
    if (l.empty())
        return false;
    if (l[0].get_value("autoindex").empty())
        return false;
    std::string autoindex = l[0].get_value("autoindex")[0];
    if (autoindex != "on")
        return false;
    return true;
}

void Response::makeAutoindex(Request const &req, Client &client) {
      _statusCode = 200;
      try {
        _body = HttpAutoindex::generateIndex(req, _path);
        setHeader("Content-Length", sizeToStr(_body.size()), true);
        setHeader("Content-Type", "text/html", true);
      } catch (HttpAutoindex::FolderRedirect const &e) {
        _statusCode = 301;
        _body = "";
        _responseHeaders.clear();
        setHeader("Location", HttpAutoindex::rewriteLocation(req.getAbsPath()),
                  true);

      } catch (HttpAutoindex::NoPathException const &e) {
        setBodyError(404, errPage(client, 404));
      }
      client.setState(C_RES);
}

void Response::processRequest(Request &req, Client &client) {
  Configuration conf(client.getConfig());
  int requestStatus = req.validateRequest(client);
  if (requestStatus) {
    setBodyError(requestStatus, errPage(client, requestStatus));
    client.setState(C_RES);
    return;
  }
  req.checkCGI(client);
  setHeader("Set-Cookie", client.cookie().full(), true);
  _statusCode = 200;
  if (!client.getConfig().is_a_allowed_Method(req.line().getMethod(),
                                              req.getFilePath())) {
    _statusCode = 405;
    logItem("is allowed", req.getFilePath());
    setBodyError(_statusCode, errPage(client, _statusCode));
    client.setState(C_RES);
    return;
  }
  HttpRedirect::handleRedirect(req, *this, conf);
  if (req.line().getMethod() == "GET") {
    if (isAutoindex(req, conf)) {
      makeAutoindex(req, client);
      return;
    }
    HttpMethod::get(req, *this);
  } else if (req.line().getMethod() == "POST" && _statusCode < 400) {
    HttpMethod::post(req, *this, conf.get_path_upload());
  } else if (req.line().getMethod() == "DELETE" && _statusCode < 400) {
    HttpMethod::del(req, *this);
  }
  if (_statusCode >= 200 && _statusCode < 300) {
    setHeader("Content-Length", sizeToStr(_body.size()), true);
    setHeader("Content-Type", findContentType(), true);
  } else {
    setBodyError(_statusCode, errPage(client, _statusCode));
  }
  if (_cgi.isRunning())
    client.setState(C_CGI);
  else
    client.setState(C_RES);
}

void Response::setBodyError(int status, std::string error_page) {
  if (error_page.empty()) {
    std::ostringstream s;
    _statusCode = status;
    s << "<!DOCTYPE html>\n<html>\n<head>\n<title>" << _statusCode << " "
      << getResponse(_statusCode) << "</title>\n</head>\n<body>\n<center>\n<h1>"
      << _statusCode << " " << getResponse(_statusCode) << "</h1>\n</center>\n"
      << "<hr>\n<center>webserv/0.1</center>" << "</body>\n</html>";
    _body = s.str();
  } else {
    setBody(error_page);
  }
  setHeader("Content-Length", sizeToStr(_body.size()), true);
  setHeader("Content-Type", "text/html", true);
}

void Response::setBody(std::string const &filename) {
  std::ifstream file(filename.c_str(),
                     std::ios::in | std::ios::binary | std::ios::ate);
  if (!file.good()) {
    _statusCode = 403;
    _body = "";
    return;
  }
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
  else if (type == "gif")
    return "image/gif";
  else if (type == "webp")
    return "image/webp";
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
  if (_body.size() > 1024 ||
      _responseHeaders.find("Content-Length") == _responseHeaders.end()) {
    setHeader("Transfer-Encoding", "chunked", true);
    setHeader("Connection", "keep-alive", true);
    res << writeHeader();
    sendStr(clientSocket, res.str());
    res.clear();
    while (_offset < _body.size()) {
      std::string chunk = chunkResponse();
      int tmp = sendChunk(clientSocket, chunk);
      _bytes_sent += tmp;
    }
    std::string chunk = chunkResponse();
    sendStr(clientSocket, chunk);
  } else {
    res << writeHeader() << _body;
    if (DEBUG)
      std::cout << writeHeader() << _body << std::endl;
    sendStr(clientSocket, res.str());
  }
}

CgiHandler Response::cgi() { return _cgi; }
