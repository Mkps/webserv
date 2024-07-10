#include "HttpRedirect.hpp"
#include "Configuration.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "http_utils.hpp"
#include <iostream>
#include <vector>
HttpRedirect::HttpRedirect() {}

HttpRedirect::~HttpRedirect() {}

HttpRedirect::HttpRedirect(HttpRedirect const &src) { (void)src; }

HttpRedirect &HttpRedirect::operator=(HttpRedirect const &rhs) {
  (void)rhs;
  return *this;
}

void HttpRedirect::handleRedirect(Request const &req, Response &response) {
  Configuration conf("{location /{index index.html;};root ./resources/;}", 0);
  conf.show();
  std::string root = conf.get_value("root")[0];
  std::string path = root + req.getFilePath();
  if (req.getFilePath() == "/funny") {
    std::cout << "Never gonna give you up" << std::endl;
    response._statusCode = 302;
    response.setHeader("Location",
                       "https://www.youtube.com/watch?v=dQw4w9WgXcQ", true);
    return;
  }
  response._path = path;
  if (req.isCGI())
    return;
  // Check for redirection here
  std::vector<std::string> tryFiles(conf.get_locations()[0].get_value("index"));
  if (fileStatus(path) == FILE_REG) {
    response._path = std::string(path);
    return;
  }
  if (fileStatus(path) == FILE_DIR) {
    for (size_t i = 0; i < tryFiles.size(); ++i) {
      std::string filePath(path + tryFiles[i]);
      if (fileStatus(filePath) == FILE_REG) {
        response._statusCode = 200;
        response._path = std::string(filePath);
        return;
      } else {
        response._statusCode = 403;
      }
    }
  } else if (fileStatus(path) == FILE_NOT) {
    response._statusCode = 404;
  }
}

bool HttpRedirect::isRedirect() { return _redirect; }

std::string const &HttpRedirect::location() { return _location; }
