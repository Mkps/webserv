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
typedef std::map<std::string, std::string> hashmap;
class CgiHandler {
private:
  hashmap _data;
  char **_envv;
  std::string _qData;
  std::string _script;
  std::string _body;

  hashmap _setEnvGet(const std::string &script, const std::string &query);
  char **_setEnvPost();
  void _execCGIGet();

public:
  CgiHandler();
  CgiHandler(std::string const &script, std::string const &query);
  ~CgiHandler();
  CgiHandler(CgiHandler const &src);
  CgiHandler &operator=(CgiHandler const &rhs);

  void setEnvGet(std::string const &script, std::string const &query);
  void setEnvPost(std::string const &script, std::string const &query);
  void setScript(std::string const &script);
  void setQueryData(std::string const &qData);
  std::string const & body() const;
  int handleGet();
  int handlePost();
};
#endif
