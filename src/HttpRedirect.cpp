#include "HttpRedirect.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "http_utils.hpp"
#include <vector>
#include <iostream>
HttpRedirect::HttpRedirect(){
}

HttpRedirect::~HttpRedirect(){
}

HttpRedirect::HttpRedirect(HttpRedirect const &src){
    (void)src;
}

HttpRedirect& HttpRedirect::operator=(HttpRedirect const &rhs){
    (void)rhs;
    return *this;
}

void HttpRedirect::handleRedirect(Request const &req, Response &response){
  std::string myStr[] = {"index", "index.html"};
  std::string root = "./resources";
  std::string path = root + req.getFilePath();
  if (req.getFilePath() == "/funny") {
      std::cout << "Never gonna give you up" << std::endl;
      response._statusCode = 302;
      response.setHeader("Location", "https://www.youtube.com/watch?v=dQw4w9WgXcQ", true);
      return ;
  }
  response._path = path;
  if (req.isCGI())
    return;
  // Check for redirection here
  std::vector<std::string> tryFiles;
  tryFiles.assign(myStr, myStr + 2);
  if (fileStatus(path) == FILE_REG) {
    response._path = std::string(path);
    return;
  }
  if (fileStatus(path) == FILE_DIR) {
    for (std::vector<std::string>::const_iterator it = tryFiles.begin();
         it != tryFiles.end(); ++it) {
      std::string filePath(path + *it);
      if (fileStatus(filePath) == FILE_REG) {
        response._path = std::string(filePath);
        return;
      } else if (fileStatus(filePath) != FILE_NOT) {
        response._statusCode = 403;
      }
    }
  } else if (fileStatus(path) == FILE_NOT) {
    response._statusCode = 404;
  }
    
}

bool HttpRedirect::isRedirect(){
    return _redirect;
}

std::string const &HttpRedirect::location(){
    return _location;
}

