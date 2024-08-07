/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:15:48 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/27 14:15:58 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

// Private Member Functions
void CgiHandler::_setEnvGet(const std::string &script,
                            const std::string &query) {
  _env["GATEWAY_INTERFACE"] = "CGI/1.1";
  _env["PATH_INFO"] = script + query;
  _env["QUERY_STRING"] = query;
  _env["REQUEST_METHOD"] = "GET";
  _env["SCRIPT_FILENAME"] = script;
  _env["REDIRECT_STATUS"] = "CGI";
  _env["SERVER_PROTOCOL"] = "HTTP/1.1";
  _env["REMOTE_ADDR"] = "127.0.0.1";
  _env["CONTENT_LENGTH"] = "0";
  _env.erase("CONTENT_TYPE");
}

void CgiHandler::_setEnvPost(const std::string &script,
                             const std::string &query) {
  _env["GATEWAY_INTERFACE"] = "CGI/1.1";
  _env["PATH_INFO"] = script + query;
  _env["QUERY_STRING"] = query;
  _env["REQUEST_METHOD"] = "POST";
  _env["SCRIPT_FILENAME"] = script;
  _env["REDIRECT_STATUS"] = "CGI";
  _env["SERVER_PROTOCOL"] = "HTTP/1.1";
  _env["REMOTE_ADDR"] = "127.0.0.1";
}

// Public Member Functions
//
// ### Constructors/Coplien
CgiHandler::CgiHandler() {
  _status = 0;
  _envv = NULL;
  _err = 0;
  _isRunning = false;
}

CgiHandler::CgiHandler(std::string const &uri) {
  _status = 0;
  _envv = NULL;
  _isRunning = false;
  _script = uri;
  _err = 0;
  size_t query_pos = _script.find("?");
  if (query_pos != std::string::npos) {
    _qData = _script.substr(query_pos + 1);
    _script = _script.substr(0, query_pos);
  }
}
CgiHandler::CgiHandler(std::string const &script, std::string const &query) {
  _status = 0;
  _envv = NULL;
  _script = script;
  _qData = query;
  _err = 0;
}

CgiHandler::~CgiHandler() {
  if (_envv) {
    for (int i = 0; _envv[i] != NULL; ++i) {
      delete[] _envv[i];
    }
    delete[] _envv;
    _envv = NULL;
  }
  _env.clear();
}

void CgiHandler::freeEnvv() {
  if (_envv) {
    for (int i = 0; _envv[i] != NULL; ++i) {
      delete[] _envv[i];
    }
    delete[] _envv;
    _envv = NULL;
  }
}
CgiHandler::CgiHandler(CgiHandler const &src) { *this = src; }

CgiHandler &CgiHandler::operator=(CgiHandler const &rhs) {
  _envv = NULL;
  _isRunning = rhs._isRunning;
  _script = rhs._script;
  _qData = rhs._qData;
  return *this;
}

inline int set_socket_nonblocking(int sockfd) {
  int flags = fcntl(sockfd, F_GETFL, 0);
  if (flags == -1) {
    // Handle error
    return -1;
  }

  if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
    // Handle error
    return -1;
  }

  return 0;
}

void CgiHandler::_execCGIGet() {
  _setEnvGet(_script, _qData);
  _envv = hashmapToChrArray(_env);
  std::string cmd = _cgiBin;
  if (cmd.empty())
    cmd = _script;
  char *script_array[3];
  script_array[0] = new char[cmd.size() + 1];
  script_array[0][cmd.size()] = 0;
  script_array[0] = strcpy(script_array[0], cmd.c_str());
  if (cmd != _script) {
    script_array[1] = new char[_script.size() + 1];
    script_array[1][_script.size()] = 0;
    script_array[1] = strcpy(script_array[1], _script.c_str());
  } else {
    script_array[1] = NULL;
  }
  script_array[2] = NULL;
  execve(cmd.c_str(), script_array, _envv);
  delete[] script_array[0];
  if (cmd != _script)
    delete[] script_array[1];
  _err = 1;
  throw std::runtime_error("500 execve error");
}

int CgiHandler::handleGet() {
  if (access(_script.c_str(), F_OK | X_OK) && _cgiBin.empty())
    return 403;
  if (pipe(_pipefd) == -1) {
    std::cerr << "pipe failed" << std::endl;
    return 500;
  }
  gettimeofday(&_startTime, NULL);
  _pid = fork();
  if (_pid < 0) {
    std::cerr << "Fork failed" << std::endl;
    return 500;
  }
  if (!_pid) {
    close(_pipefd[0]);
    dup2(_pipefd[1], STDOUT_FILENO);
    close(_pipefd[1]);
    _execCGIGet();
  } else {
    close(_pipefd[1]);
    set_socket_nonblocking(_pipefd[0]);
    _isRunning = false;
    int child_status = waitpid(_pid, &_status, WNOHANG | WUNTRACED);
    if (child_status == -1) {
      _isRunning = false;
      return 500;
    } else if (child_status == 0) {
      _isRunning = true;
    } else {
      int n = 0;
      char buffer[1024];
      while ((n = read(_pipefd[0], buffer, sizeof(buffer))) > 0) {
        _body.append(buffer, n);
      }
      close(_pipefd[0]);
    }
  }
  return 200;
}

int CgiHandler::getProcessState() {
  int process_status;
  process_status = waitpid(_pid, &_status, WNOHANG | WUNTRACED);
  if (process_status) {
    _isRunning = false;
    if (process_status > 0) {
      char buffer[1024];
      _body = "";
      int bytesRead;
      while ((bytesRead = read(_pipefd[0], buffer, sizeof(buffer))) > 0) {
        _body.append(buffer, bytesRead);
      }
    }
    close(_pipefd[0]);
  }
  return process_status;
}

int CgiHandler::getStatus() {
  if (_err == 1)
    return 500;
  if (WIFEXITED(_status)) {
    return 200;
  }
  if (WIFSIGNALED(_status)) {
    return 500;
  }
  return 444;
}
bool CgiHandler::isRunning() { return _isRunning; }

int CgiHandler::timeout() {
  timeval_t currentTime;
  gettimeofday(&currentTime, NULL);
  double elapsedTime = (currentTime.tv_sec - _startTime.tv_sec) +
                       (currentTime.tv_usec - _startTime.tv_usec) / 1000000.0;
  if (elapsedTime > CGI_TIMEOUT)
    return 1;
  return 0;
}

void CgiHandler::killCgi() {
  kill(_pid, SIGKILL);
  close(_pipefd[0]);
}

void CgiHandler::_execCGIPost() {
  _setEnvPost(_script, _qData);
  _envv = hashmapToChrArray(_env);
  std::string cmd = _cgiBin;
  if (cmd.empty())
    cmd = _script;
  char *script_array[3];
  script_array[0] = new char[cmd.size() + 1];
  script_array[0][cmd.size()] = 0;
  script_array[0] = strcpy(script_array[0], cmd.c_str());
  if (cmd != _script) {
    script_array[1] = new char[_script.size() + 1];
    script_array[1][_script.size()] = 0;
    script_array[1] = strcpy(script_array[1], _script.c_str());
  } else {
    script_array[1] = NULL;
  }
  script_array[2] = NULL;
  std::cerr << "POST EXEC" << std::endl;
  execve(cmd.c_str(), script_array, _envv);
  delete[] script_array[0];
  if (cmd != _script)
    delete[] script_array[1];
  _err = 1;
  throw std::runtime_error("500 execve error");
}

int CgiHandler::handlePost() {
  if (pipe(_pipefd)) {
    std::cerr << "Pipe failed" << std::endl;
    return 500;
  }
  gettimeofday(&_startTime, NULL);
  _pid = fork();
  if (_pid < 0) {
    std::cerr << "Fork failed" << std::endl;
    return 500;
  }

  if (_pid == 0) {
    dup2(_pipefd[0], STDIN_FILENO);
    dup2(_pipefd[1], STDOUT_FILENO);
    close(_pipefd[0]);
    close(_pipefd[1]);
    _execCGIPost();
  } else {
    int bytesSent = 0;
    int tmp;
    set_socket_nonblocking(_pipefd[1]);
    while (bytesSent < static_cast<int>(_requestBody.size())) {
      tmp = write(_pipefd[1], _requestBody.c_str() + bytesSent,
                  _requestBody.size() - bytesSent);
      if (tmp > 0)
        bytesSent += tmp;
    }
    close(_pipefd[1]);
    set_socket_nonblocking(_pipefd[0]);
    _body = "";
    _isRunning = true;
    int child_status = waitpid(_pid, &_status, WNOHANG | WUNTRACED);
    if (child_status == -1) {
      _isRunning = false;
      return 500;
    }
  }
  return 200;
}

inline char tocgi(char c) {
  if (isalpha(c))
    return (toupper(c));
  else if (c == '-')
    return '_';
  else
    return c;
}
// ### Getters/Setters ###/
std::string const &CgiHandler::body() const { return _body; }

void CgiHandler::setHeaders(hashmap const &requestHeaders) {
  hashmap::const_iterator it = requestHeaders.begin();
  for (; it != requestHeaders.end(); ++it) {
    std::string tmp = it->first;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tocgi);
    if (tmp == "CONTENT_LENGTH" || tmp == "CONTENT_TYPE") {
      _env[tmp] = it->second;
    }
  }
}
void CgiHandler::setRequestBody(std::string const &requestbody) {
  _requestBody = requestbody;
}

void setScript(std::string const &script);
void CgiHandler::setCgiBin(std::string const &cgiBin) { _cgiBin = cgiBin; }

void setQueryData(std::string const &qData);
int CgiHandler::getFd() { return _pipefd[0]; }

// Helper function
inline char **hashmapToChrArray(hashmap const &map) {
  char **ret;
  ret = new char *[map.size() + 1];
  ret[map.size()] = NULL;
  int i = -1;
  for (hashmap::const_iterator it = map.begin(); it != map.end(); ++it) {
    std::string tmp = it->first + "=" + it->second;
    ret[++i] = new char[tmp.size() + 1];
    ret[i][tmp.size()] = 0;
    ret[i] = strcpy(ret[i], tmp.c_str());
  }
  return ret;
}
