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

Response::Response() {
  // If this is used something went very wrong...
  setDefaultHeaders();
}

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
  setHeader("Content-Type", "text/plain");
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
  // Should unset the map
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
    // res << "HTTP/1.1 200 OK\r\n"
    //          << "Date: " << get_current_date() << "\r\n"
    //          << "Content-Type: text/html; charset=UTF-8\r\n"
    //          << "Content-Length: " << _body.size() << "\r\n"
    //          << "Connection: close\r\n"  // Close the connection after
    //          sending the response
    //          << "Server: MyServer/1.0\r\n"
    //          << "\r\n"
    //          << _body;
    std::cout << "response is >>> " << res.str() << std::endl;
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

inline hashmap setEnvForCGI(std::string const &script, std::string const &query)
{
	hashmap tmp;
    tmp["GATEWAY_INTERFACE"] = "CGI/1.1";
    tmp["QUERY_STRING"] = query;
    tmp["REQUEST_METHOD"] = "GET";
    tmp["SCRIPT_FILENAME"] = script;
    tmp["SERVER_PROTOCOL"] =  "HTTP/1.1";
    tmp["REMOTE_ADDR"] = "127.0.0.1";
	return tmp;	
}

inline char** hashmapToChrArray(hashmap const &map) {
	char **ret;
	ret = new char*[map.size() + 1];
	ret[map.size()] = NULL;
	int i = -1;
	for (hashmap::const_iterator it = map.begin() ; it != map.end(); ++it) {
		std::string tmp = it->first + "=" + it->second;
		ret[++i] = new char[tmp.size() + 1];
		ret[i][tmp.size()] = 0;
		ret[i] = strcpy(ret[i], tmp.c_str()); 
	}
	return ret;
}
int Response::handleCGI(int clientSocket, std::string const &script,
                         std::string const &query) {
  (void)clientSocket;
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    std::cerr << "pipe failed" << std::endl;
    return 500;
  }
  pid_t pid = fork();
  if (pid < 0) {
    std::cerr << "Fork failed" << std::endl;
    return 500;
  }
  if (!pid) {
    close(pipefd[0]);
	hashmap env;
	env = setEnvForCGI(script, query);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);
	std::cerr<< "script " << script;
	char **envv = hashmapToChrArray(env);
	char *script_array[2];
	script_array[1] = NULL;
	script_array[0] = new char[script.size() + 1];
	script_array[0][script.size()] = 0;
	script_array[0] = strcpy(script_array[0],script.c_str());
    execve(script.c_str(), script_array, envv);
    for (int i = 0; envv[i]; ++i){
        std::cerr << "env i " << i << " >>>" << envv[i] << std::endl;
        delete[] envv[i];
    }
	delete[] envv;
    _exit(127);
  } else {
    close(pipefd[1]);
    int status;
    if (waitpid(pid, &status, 0) == -1)
      return 500;
    if (WIFEXITED(status) && WEXITSTATUS(status))
      return 502;
    char buffer[4096];
    _body = "";
    ssize_t n;
    while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
      _body.append(buffer, n);
    }
    close(pipefd[0]);
    setHeader("Content-Length", sizeToStr(_body.size()));
    setHeader("Content-Type", findContentType());
  }
  return 200;
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
  if (_statusCode == 200 && req.isCGI()) {
    int ret = handleCGI(0, script_path, query);
    if (ret != 200)
        setBodyError(ret);
  } else if (_statusCode == 200) { // We have a valid file
    setBody(_path);
    setHeader("Content-Length", sizeToStr(_body.size()));
    setHeader("Content-Type", findContentType());
    std::cout << "path " << _path << " size " << sizeToStr(_body.size())
              << " type " << findContentType() << std::endl;
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
