#pragma once

#include <string>
class Response;
class Request;
// Used as an interface between the request and the response allowing to define
// the location
class HttpRedirect {
public:
  HttpRedirect();
  ~HttpRedirect();
  HttpRedirect(HttpRedirect const &src);
  HttpRedirect &operator=(HttpRedirect const &rhs);
  static void handleRedirect(Request const &req, Response &response);
  bool isRedirect();
  std::string const &location();

private:
  std::string _location;
  bool _redirect;
};
