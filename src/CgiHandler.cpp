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
#include "http_utils.hpp"
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
hashmap CgiHandler::_setEnvGet(const std::string &script,
                               const std::string &query) {
  hashmap tmp;
  tmp["GATEWAY_INTERFACE"] = "CGI/1.1";
  tmp["QUERY_STRING"] = query;
  tmp["REQUEST_METHOD"] = "GET";
  tmp["SCRIPT_FILENAME"] = script;
  tmp["SERVER_PROTOCOL"] = "HTTP/1.1";
  tmp["REMOTE_ADDR"] = "127.0.0.1";
  return tmp;
}

hashmap CgiHandler::_setEnvPost(const std::string &script,
                                const std::string &query) {
  hashmap tmp;
  tmp["CONTENT_LENGTH"] = sizeToStr(_requestBody.size());
  tmp["GATEWAY_INTERFACE"] = "CGI/1.1";
  tmp["QUERY_STRING"] = query;
  tmp["REQUEST_METHOD"] = "POST";
  tmp["SCRIPT_FILENAME"] = script;
  tmp["SERVER_PROTOCOL"] = "HTTP/1.1";
  tmp["REMOTE_ADDR"] = "127.0.0.1";
  return tmp;
}

// Public Member Functions
//
// ### Constructors/Coplien
CgiHandler::CgiHandler() {
  _envv = NULL;
  _isRunning = false;
}

CgiHandler::CgiHandler(std::string const &uri) {
  _envv = NULL;
  _isRunning = false;
  _script = uri;
  size_t query_pos = _script.find("?");
  if (query_pos != std::string::npos) {
    _qData = _script.substr(query_pos + 1);
    _script = _script.substr(0, query_pos);
  }
}
CgiHandler::CgiHandler(std::string const &script, std::string const &query) {
  _envv = NULL;
  _script = script;
  _qData = query;
}

CgiHandler::~CgiHandler() {
  if (_envv) {
    for (int i = 0; _envv[i] != NULL; ++i) {
      delete[] _envv[i];
    }
    delete[] _envv;
    _envv = NULL;
  }
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
  hashmap env;
  env = _setEnvGet(_script, _qData);
  _envv = hashmapToChrArray(env);
  /* exit(127); */
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
  throw std::runtime_error("502 execve error");
}
int CgiHandler::handleGet() {
  if (access(_script.c_str(), F_OK | X_OK))
    return 403;
  if (pipe(_pipefd) == -1) {
    std::cerr << "pipe failed" << std::endl;
    return 500;
  }
  gettimeofday(&_startTime, NULL);
  _pid = fork();
  _isRunning = true;
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
    char buffer[4096];
    _body = "";
    size_t n;
    int child_status = waitpid(_pid, &_status, WNOHANG | WUNTRACED);
    std::cout << "child status " << child_status << std::endl;
    if (child_status == -1) {
      _isRunning = false;
      return 500;
    } else if (child_status == 0) {
      _isRunning = true;
    } else {
      while ((n = read(_pipefd[0], buffer, sizeof(buffer))) > 0) {
        /*std::cout << "reading " << buffer << std::endl;*/
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
  if (process_status)
    _isRunning = false;
  if (process_status > 0) {
    char buffer[1024];
    _body = "";
    int bytesRead;
    while ((bytesRead = read(_pipefd[0], buffer, sizeof(buffer))) > 0) {
      _body.append(buffer, bytesRead);
    }
    close(_pipefd[0]);
  }
  return process_status;
}

int CgiHandler::getStatus() {
  if (WIFEXITED(_status)) {
    if (WEXITSTATUS(_status))
      return 502;
    else if (!WEXITSTATUS(_status))
      return 200;
  }
  return 444;
}
bool CgiHandler::isRunning() { return _isRunning; }

int CgiHandler::timeout() {
  timeval_t currentTime;
  gettimeofday(&currentTime, NULL);
  double elapsedTime = (currentTime.tv_sec - _startTime.tv_sec) +
                       (currentTime.tv_usec - _startTime.tv_usec) / 1000000.0;
  std::cout << "elapsed time " << elapsedTime << std::endl;
  if (elapsedTime > CGI_TIMEOUT)
    return 1;
  return 0;
}

void CgiHandler::killCgi() {
  kill(_pid, SIGKILL);
  close (_pipefd[0]);
  std::cerr << "KILLING CGI" << std::endl;
}

void CgiHandler::_execCGIPost() {
  hashmap env;
  env = _setEnvPost(_script, _qData);
  _envv = hashmapToChrArray(env);
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
  std::cout << "POST EXEC" << std::endl;
  execve(cmd.c_str(), script_array, _envv);
  if (access(_script.c_str(), F_OK | X_OK)) {
    exit(126);
  }
  delete[] script_array[0];
  if (cmd != _script)
    delete[] script_array[1];
  execve(_script.c_str(), script_array, _envv);
  delete[] script_array[0];
  throw std::runtime_error("502 execve error");
}
int CgiHandler::handlePost() {
  int stdinPipe[2];

  if (pipe(stdinPipe) || pipe(_pipefd)) {
    std::cerr << "Pipe failed" << std::endl;
    return 500;
  }
  pid_t pid = fork();
  if (pid < 0) {
    std::cerr << "Fork failed" << std::endl;
    return 500;
  }

  if (pid == 0) {
    close(stdinPipe[1]);
    dup2(stdinPipe[0], STDIN_FILENO);
    close(stdinPipe[0]);

    close(_pipefd[0]);
    dup2(_pipefd[1], STDOUT_FILENO);
    close(_pipefd[1]);

    _execCGIPost();
  } else {
    close(stdinPipe[0]);
    write(stdinPipe[1], _requestBody.c_str(), _requestBody.size());
    close(stdinPipe[1]);
    close(_pipefd[1]);
    set_socket_nonblocking(_pipefd[0]);
    char buffer[4096];
    _body = "";
    size_t n;
    int child_status = waitpid(_pid, &_status, WNOHANG | WUNTRACED);
    std::cout << "child status " << child_status << std::endl;
    if (child_status == -1) {
      _isRunning = false;
      return 500;
    } else if (child_status == 0) {
      _isRunning = true;
    } else {
      while ((n = read(_pipefd[0], buffer, sizeof(buffer))) > 0) {
        /*std::cout << "reading " << buffer << std::endl;*/
        _body.append(buffer, n);
      }
      close(_pipefd[0]);
    }
  }
  return 200;
}

// ### Getters/Setters ###/
std::string const &CgiHandler::body() const { return _body; }

void CgiHandler::setRequestBody(std::string const &requestbody) {
  _requestBody = requestbody;
}

void setScript(std::string const &script);
void CgiHandler::setCgiBin(std::string const &cgiBin) { _cgiBin = cgiBin; }

void setQueryData(std::string const &qData);
int CgiHandler::getFd(){
    return _pipefd[0];
}

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
