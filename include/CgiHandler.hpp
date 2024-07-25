/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:15:38 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/27 14:15:39 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include <map>
#include <string>
#include <sys/types.h>
#include <sys/time.h>

#define CGI_TIMEOUT 3

typedef std::map<std::string, std::string> hashmap;
typedef struct timeval timeval_t;
class CgiHandler {
private:
  hashmap _data;
  char **_envv;
  int _pipefd[2];
  pid_t _pid;
  int _status;
  timeval_t _startTime;
  std::string _cgiBin;
  std::string _qData;
  std::string _script;
  std::string _body;
  std::string _requestBody;
  bool _isRunning;

  hashmap _setEnvGet(const std::string &script, const std::string &query);
  hashmap _setEnvPost(const std::string &script, const std::string &query);
  void _execCGIGet();
  void _execCGIPost();
  void freeEnvv();

public:
  CgiHandler();
  CgiHandler(std::string const &script, std::string const &query);
  CgiHandler(std::string const &uri);
  ~CgiHandler();
  CgiHandler(CgiHandler const &src);
  CgiHandler &operator=(CgiHandler const &rhs);

  int handleGet();
  int handlePost();

  std::string const &body() const;
  void setRequestBody(std::string const &requestbody);
  void setScript(std::string const &script);
  void setCgiBin(std::string const &cgiBin);
  void setQueryData(std::string const &qData);
  bool isRunning();
  int getProcessState();
  int getStatus();
  int timeout();
  int getFd();
  void killCgi();
};
char **hashmapToChrArray(hashmap const &map);
#endif
