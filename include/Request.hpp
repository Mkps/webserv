/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:05 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/17 14:51:06 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "RequestLine.hpp"
#include <map>

typedef std::map<std::string, std::string> hashmap;
class Request {
private:
  RequestLine 	_requestLine;
  hashmap		_requestHeaders;
  std::string	_body;
  void			retrieveHeaders(std::string const &request);
  void			fetchData(std::string const &request);
  int			initData(); // will eventually be taken over by Server->conf

public:
  Request();
  Request(std::string const &request);
  ~Request();
  Request(Request const &src);
  Request &operator=(Request const &rhs);

  int					validateRequest() const;
  std::string			getFilePath() const;
  void 					setRequest(std::string const &request);
  Request const 		&getRequest() const;
  bool					isCGI() const;
  RequestLine const		&getRequestLine() const;
  hashmap const			&getRequestHeaders() const;
  std::string const		&getRequestBody() const;
};
#endif
