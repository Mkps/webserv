/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 15:30:28 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/18 15:30:29 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "CgiHandler.hpp"
#include "Configuration.hpp"
#include <map>
#include <string>

typedef std::map<std::string, std::string> hashmap;
class Request;
class Client;
class Configuration;
class Response {
private:
  short _statusCode;
  hashmap _responseHeaders;
  std::string _body;
  std::string _path;
  size_t _bytes_sent;
  size_t _offset;
  CgiHandler _cgi;

public:
  Response();
  Response(Request const &request);
  ~Response();
  Response(Response const &src);
  Response &operator=(Response const &rhs);

  void setStatusCode(short status);
  short const &getStatusCode() const;
  std::string const &getContentType() const;
  size_t const &getContentLength() const;

  std::string getResponseMsg();
  void setBodyError(int status, std::string error_page);
  void setBody(std::string const &filename);
  hashmap const &getHeader() const;
  std::string getHeaderValue(std::string const &key) const;
  void setHeader(std::string const &key, std::string const &value,
                 bool overwrite);
  void setDefaultHeaders();

  void clear();
  std::string writeHeader();
  void sendResponse(int clientSocket);
  std::string chunkResponse();

  int handleCGI(int clientSocket, std::string const &script,
                std::string const &query);
  void processRequest(Request &req, Client &client);
  void processCgi(Request &req, Client &client);
  bool isAutoindex(Request const &req, Configuration const &conf);
  std::string findContentType();
  void findPath(Request const &req);
  CgiHandler cgi();

  friend class HttpRedirect;
  friend class HttpMethod;
};
#endif
