/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLine.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 18:46:03 by aloubier          #+#    #+#             */
/*   Updated: 2024/08/01 18:46:04 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTLINE_HPP
#define REQUESTLINE_HPP
#include <string>

class RequestLine {
private:
  std::string _method;
  std::string _requestUri;
  std::string _httpVersion;

public:
  RequestLine();
  RequestLine(std::string const &requestLine);
  RequestLine(RequestLine const &src);
  RequestLine &operator=(RequestLine const &rhs);
  ~RequestLine();

  std::string const &getMethod() const;
  void setMethod(std::string &method);
  std::string const &getRequestUri() const;
  void setRequestUri(std::string &Uri);
  std::string const &getHttpVersion() const;
  void setHttpVersion(std::string &Uri);

  int isVersionValid() const;
  bool isURIValid() const;
  bool isRLValid() const;
};
#endif
