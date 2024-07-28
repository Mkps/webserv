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
#include "Cookie.hpp"
#include "HttpAutoindex.hpp"
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

inline std::string errPage(Client &client, size_t error_code) {
  return client.getConfig().get_error_page(error_code);
}
inline std::string getResponse(short status) {
  if (status >= 100 && status < 200) {
    return "Informational";
  } else if (status >= 200 && status < 300) {
    if (status == 201)
      return "Created";
    else if (status == 204)
      return "No Response";
    return "OK";
  } else if (status >= 300 && status < 400) {
    if (status == 301)
      return "Moved Permanently";
    else if (status == 302)
      return "Found";
    return "Redirection";
  } else if (status >= 400 && status < 500) {
    if (status == 400)
      return "Bad Request";
    else if (status == 403)
      return "Forbidden";
    else if (status == 404)
      return "Not Found";
    else if (status == 405)
      return "Method Not Allowed";
    else if (status == 406)
      return "Not Acceptable";
    else if (status == 411)
      return "Length Required";
    else if (status == 414)
      return "Request-URI Too Long";
    else
      return "Client Error";
  } else if (status >= 500 && status < 600) {
    if (status == 502)
      return "Bad Gateway";
    if (status == 504)
      return "Gateway Timeout";
    return "Internal Server Error";
  } else
    return "Unidentified Error";
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

void Response::processRequest(Request &req, Client &client) {
  Configuration conf(client.getConfig());
  conf.show();
  int requestStatus = req.validateRequest(client);
  if (requestStatus) {
    setBodyError(requestStatus, errPage(client, requestStatus));
    client.setState(C_RES);
    return;
  }
  req.checkCGI(client);
  setHeader("Set-Cookie", client.cookie().full(), true);
  _statusCode = 200;
  if (!client.getConfig().is_a_allowed_Method(req.line().getMethod(), req.getFilePath())) {
    _statusCode = 405;
    logItem("is allowed", req.getFilePath());
    setBodyError(_statusCode, errPage(client, _statusCode));
    client.setState(C_RES);
    return;
  }
  HttpRedirect::handleRedirect(req, *this, client.getConfig());
  if (req.line().getMethod() == "GET") {
    if (_statusCode == 403 &&
        !client.getConfig()
             .get_locations_by_path("/")[0]
             .get_value("autoindex")
             .empty() &&
        client.getConfig().get_locations_by_path("/")[0].get_value(
            "autoindex")[0] == "on") {
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
      return;
    }
    httpMethodGet(req);
    if (_cgi.isRunning())
      client.setState(C_CGI);
    else
      client.setState(C_RES);
  } else if (req.line().getMethod() == "POST" && _statusCode < 400) {
    httpMethodPost(req, client.getConfig().get_path_upload());
    if (_cgi.isRunning())
      client.setState(C_CGI);
    else
      client.setState(C_RES);
  } else if (req.line().getMethod() == "DELETE" && _statusCode < 400) {
    httpMethodDelete(req);
  }
  if (_statusCode >= 200 && _statusCode < 300) {
    setHeader("Content-Length", sizeToStr(_body.size()), true);
    setHeader("Content-Type", findContentType(), true);
  } else {
    setBodyError(_statusCode, errPage(client, _statusCode));
  }
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
    if (!_body.empty())
      _body += "\r\n\r\n";
    res << writeHeader() << _body;
    if (DEBUG)
      std::cout << writeHeader() << _body << std::endl;
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

void Response::httpMethodGet(Request const &req) {
  if (_statusCode == 200 && req.isCGI()) {
    CgiHandler cgi(_path);
    _cgi = cgi;
    _cgi.setCgiBin(req.getCgiPath());
    _statusCode = _cgi.handleGet();
    if (_statusCode == 200)
      _body = _cgi.body();
  }
  if (!req.isCGI() && _statusCode == 200) {
    if (fileStatus(_path) == FILE_REG && !access(_path.c_str(), F_OK | R_OK))
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

inline std::string extractBoundary(const std::string &contentType) {
  std::string boundaryPrefix = "boundary=";
  size_t boundaryPos = contentType.find(boundaryPrefix);
  if (boundaryPos != std::string::npos) {
    return "--" + contentType.substr(boundaryPos + boundaryPrefix.size());
  }
  return "";
}

inline std::vector<std::string> splitParts(const std::string &body,
                                           const std::string &boundary) {
  std::vector<std::string> parts;
  size_t pos = 0;
  size_t end;
  if (boundary.empty()) {
    parts.push_back(body);
    return parts;
  }
  while ((end = body.find(boundary, pos)) != std::string::npos) {
    parts.push_back(body.substr(pos, end - pos));
    pos = end + boundary.size();
  }
  if (pos < body.size()) {
    parts.push_back(body.substr(pos));
  }
  return parts;
}

inline std::string extractFileName(const std::string &part) {
  std::string filename;
  std::istringstream partStream(part);
  std::string line;
  size_t pos = part.find("Content-Disposition:");
  if (pos != std::string::npos) {
    size_t filenamePos = part.find("filename=", pos);
    if (filenamePos != std::string::npos) {
      size_t start = part.find('"', filenamePos) + 1;
      size_t end = part.find('"', start);
      filename = part.substr(start, end - start);
    }
  }
  return filename;
}

std::string extractContent(const std::string &part) {
  size_t headerEnd = part.find("\r\n\r\n");
  if (headerEnd != std::string::npos) {
    return part.substr(headerEnd + 4);
  }
  return "";
}

inline std::vector<std::string> get_multipart(const Request &req) {
  std::string requestBody = req.body();
  std::string contentType = req.findValue("content-type");
  if (contentType.find("multipart") == contentType.npos)
    return std::vector<std::string>();
  std::string boundary = extractBoundary(contentType);
  std::vector<std::string> parts = splitParts(requestBody, boundary);
  return parts;
}

void Response::httpMethodPost(Request const &req, std::string const &upload) {
  if (req.isCGI()) {
    CgiHandler cgi(_path);
    _cgi = cgi;
    _cgi.setRequestBody(req.body());
    _cgi.setCgiBin(req.getCgiPath());
    int ret = _cgi.handlePost();
    if (ret == 200)
      _body = cgi.body();
  } else if (_statusCode == 200) {
    std::string uploadBody = "";
    std::string fileName = "";
    std::ofstream outFile;
    std::string upload_path;
    if (!upload.empty())
      upload_path = upload;
    else
      upload_path = _path;
    if (fileStatus(upload_path) == FILE_DIR) {
      if (*upload_path.rbegin() != '/')
        upload_path += "/";
      std::vector<std::string> multipart = get_multipart(req);
      if (!multipart.empty()) {
        for (size_t i = 0; i < multipart.size(); ++i) {
          if (fileName.empty()) {
            fileName = extractFileName(multipart[i]);
          }
          if (uploadBody.empty())
            uploadBody = extractContent(multipart[i]);
        }
      } else {
        upload_path += generate_filename();
        uploadBody = req.body();
      }
    }
    if (!fileName.empty())
      upload_path += fileName;

    bool isCreated = false;
    if (access(upload_path.c_str(), F_OK | W_OK)) {
      isCreated = true;
    }
    outFile.open(upload_path.c_str(),
                 std::ios::out | std::ios::binary | std::ios::trunc);
    if (!outFile.good()) {
      _statusCode = 403;
      return;
    }
    logItem("uploadbody is", uploadBody);
    outFile.write(uploadBody.c_str(), uploadBody.size());
    std::string fileAbsPath = req.getAbsPath() + "/" + fileName;
    setHeader("Location", fileAbsPath, true);
    if (isCreated) {
      _statusCode = 201;
    } else {
      _statusCode = 204;
    }
    return;
  }
}

CgiHandler Response::cgi() { return _cgi; }
