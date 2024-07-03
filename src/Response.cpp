/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obouhlel <obouhlel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 15:30:25 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/25 14:24:51 by obouhlel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "CgiHandler.hpp"
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

void Response::setHeader(std::string const &key, std::string const &value) {
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
  std::string root = "./resources";
  std::string path = root + req.getFilePath();
  _path = path;
  setStatusCode(200);
  if (req.getRequestLine().getMethod() == "GET") {
    httpMethodGet(req);
  } else if (req.getRequestLine().getMethod() == "POST") {
    httpMethodPost(req);
  } else if (req.getRequestLine().getMethod() == "DELETE") {
    httpMethodDelete(req);
  } else {
    setStatusCode(405);
    setBodyError(_statusCode);
  }
}

void Response::setBodyError(int status) {

  std::ostringstream s;
  _statusCode = status;
  s << "<!DOCTYPE html>\n<html>\n<head>\n</head>\n<body>\n<hl>" << _statusCode
    << "</h1>\n<p>" << getResponse(_statusCode) << "</p></body>\n</html>\r\n";
  _body = s.str();
  setHeader("Content-Length", sizeToStr(_body.size()));
  setHeader("Content-Type", "text/html");
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

inline std::string to_hex(size_t value) {
  std::ostringstream oss(std::ios::binary);
  oss << std::hex << value;
  return oss.str();
}
inline int sendStr(int clientSocket, std::string const &str) {
  return send(clientSocket, str.c_str(), strlen(str.c_str()), 0);
}
inline int sendChunk(int clientSocket, std::string const &chunk) {
  int tmp = 0;
  int ret = 0;
  std::string size = to_hex(chunk.size()) + "\r\n";
  std::string chunkMsg = chunk + "\r\n";
  tmp = send(clientSocket, size.c_str(), size.size(), 0);
  if ((size_t)tmp != size.size()) {
    std::cout << "size " << tmp << std::endl;
  }
  ret += tmp;
  tmp = send(clientSocket, chunkMsg.c_str(), chunkMsg.size(), 0);
  if ((size_t)tmp != chunkMsg.size()) {
    std::cout << "chunk " << tmp << std::endl;
  }
  ret += tmp;
  return ret;
}
void Response::setDefaultHeaders() {
  setHeader("Date", get_current_date());
  setHeader("Content-Length", "42");
  setHeader("Content-Type", "text/plain");
  setHeader("Connection", "close");
  setHeader("Charset", "UTF-8");
  setHeader("Server", "webserv/0.1");
}

void Response::clear() {
  _responseHeaders.clear();
  setDefaultHeaders();
}
std::string Response::chunkResponse() {
  const size_t chunk_size = 1024; // Adjust this size as needed for the example
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
  // std::string res = writeHeader() + _body + "\r\n\r\n";
  std::ostringstream res;
  _offset = 0;
  _bytes_sent = 0;
  if (_body.size() > 1024) {
    setHeader("Transfer-Encoding", "chunked");
    setHeader("Connection", "keep-alive");
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
  std::string script_path, query;
  if (req.isCGI()) {
    script_path = path;
    size_t query_pos = script_path.find("?");
    if (query_pos != std::string::npos) {
      query = script_path.substr(query_pos + 1);
      script_path = script_path.substr(0, query_pos);
    }
    path = script_path;
  }
  tryFiles.assign(myStr, myStr + 2);
  if (fileStatus(path) == FILE_DIR) {
    for (std::vector<std::string>::const_iterator it = tryFiles.begin();
         it != tryFiles.end(); ++it) {
      std::string filePath(path + *it);
      if (fileStatus(filePath) == FILE_REG) {
        _statusCode = 200;
        path = filePath;
        break;
      } else
        setBodyError(404);
    }
  } else if (fileStatus(path) == FILE_NOT) {
    setBodyError(404);
  }
  if (_statusCode == 200)
    _path = path;
  if (_statusCode == 200 && req.isCGI()) {
    CgiHandler cgi(script_path, query);
    int ret = cgi.handleGet();
    if (ret != 200)
      setBodyError(ret);
    else
      _body = cgi.body();
    setHeader("Content-Length", sizeToStr(_body.size()));
    setHeader("Content-Type", findContentType());
  } else if (_statusCode == 200) {
    setBody(_path);
    setHeader("Content-Length", sizeToStr(_body.size()));
    setHeader("Content-Type", findContentType());
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
  std::string root = "./resources";
  std::string path = root + req.getFilePath();
  std::string script_path, query;
  if (req.isCGI() && _statusCode == 200) {
    script_path = path;
    size_t query_pos = script_path.find("?");
    if (query_pos != std::string::npos) {
      query = script_path.substr(query_pos + 1);
      script_path = script_path.substr(0, query_pos);
    }
    path = script_path;
    CgiHandler cgi(script_path, query);
    cgi.setRequestBody(req.getRequestBody());
    int ret = cgi.handlePost();
    if (ret != 200)
      setBodyError(ret);
    else
      _body = cgi.body();
    setHeader("Content-Length", sizeToStr(_body.size()));
    setHeader("Content-Type", findContentType());
  } else if (_statusCode == 200) {
    std::ofstream outFile;
    if (fileStatus(path) == FILE_DIR)
      path += generate_filename();
    outFile.open(path.c_str(), std::ios::out | std::ios::binary |
                                   std::ios::ate | std::ios::app);
    if (!outFile.good()) {
      _statusCode = 500;
      return;
    }
    outFile.write(req.getRequestBody().c_str(), req.getRequestBody().size());
    _statusCode = 204;
    return;
  } else {
    setBodyError(_statusCode);
  }
}
