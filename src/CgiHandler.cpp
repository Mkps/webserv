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
#include <iostream>
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
CgiHandler::CgiHandler() { _envv = NULL; }

CgiHandler::CgiHandler(std::string const &uri) {
  _envv = NULL;
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
    _envv =  NULL;
  }
}

void CgiHandler::freeEnvv() {
  if (_envv) {
    for (int i = 0; _envv[i] != NULL; ++i) {
      delete[] _envv[i];
    }
    delete[] _envv;
    _envv =  NULL;
  }
}
CgiHandler::CgiHandler(CgiHandler const &src) { (void)src; }

CgiHandler &CgiHandler::operator=(CgiHandler const &rhs) {
  (void)rhs;
  return *this;
}

void CgiHandler::_execCGIGet() {
  hashmap env;
  env = _setEnvGet(_script, _qData);
  _envv = hashmapToChrArray(env);
  char *script_array[2];
  script_array[1] = NULL;
  script_array[0] = new char[_script.size() + 1];
  script_array[0][_script.size()] = 0;
  script_array[0] = strcpy(script_array[0], _script.c_str());
  execve(_script.c_str(), script_array, _envv);
  freeEnvv();
  exit(127);
}

int CgiHandler::handleGet() {
  if (access(_script.c_str(), F_OK | X_OK))
      return 403;
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
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);
    _execCGIGet();
  } else {
    close(pipefd[1]);
    int status;
    if (waitpid(pid, &status, 0) == -1)
      return 500;
    if (WIFEXITED(status) && WEXITSTATUS(status))
      return 502;
    char buffer[4096];
    _body = "";
    size_t n;
    while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
      _body.append(buffer, n);
    }
    close(pipefd[0]);
  }
  return 200;
}

void CgiHandler::_execCGIPost() {
  hashmap env;
  env = _setEnvPost(_script, _qData);
  _envv = hashmapToChrArray(env);
  char *script_array[2];
  script_array[1] = NULL;
  script_array[0] = new char[_script.size() + 1];
  script_array[0][_script.size()] = 0;
  script_array[0] = strcpy(script_array[0], _script.c_str());
  if (access(_script.c_str(), F_OK | X_OK)) {
    exit(126);
  }
  int ret = execve(_script.c_str(), script_array, _envv);
  exit(ret);
}
int CgiHandler::handlePost() {
  int stdinPipe[2];
  int stdoutPipe[2];

  if (pipe(stdinPipe) || pipe(stdoutPipe)) {
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

    close(stdoutPipe[0]);
    dup2(stdoutPipe[1], STDOUT_FILENO);
    close(stdoutPipe[1]);

    _execCGIPost();
  } else {
    close(stdinPipe[0]);
    write(stdinPipe[1], _requestBody.c_str(), _requestBody.size());
    close(stdinPipe[1]);
    close(stdoutPipe[1]);

    std::string result;
    char buffer[1024];
    _body = "";
    int bytesRead;
    while ((bytesRead = read(stdoutPipe[0], buffer, sizeof(buffer))) > 0) {
      _body.append(buffer, bytesRead);
    }
    close(stdoutPipe[0]);

    int status;
    if (waitpid(pid, &status, 0) == -1)
      return 500;
    if (WIFEXITED(status) && WEXITSTATUS(status))
      return 502;
  }
  return 200;
}

// ### Getters/Setters ###/
std::string const &CgiHandler::body() const { return _body; }

void CgiHandler::setRequestBody(std::string const &requestbody) {
  _requestBody = requestbody;
}

void setScript(std::string const &script);

void setQueryData(std::string const &qData);

// Helper functions
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
