/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRedirect.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 18:43:58 by aloubier          #+#    #+#             */
/*   Updated: 2024/08/01 18:44:00 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRedirect.hpp"
#include "Configuration.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "http_utils.hpp"
#include <vector>
HttpRedirect::HttpRedirect() {}

HttpRedirect::~HttpRedirect() {}

HttpRedirect::HttpRedirect(HttpRedirect const &src) { (void)src; }

HttpRedirect &HttpRedirect::operator=(HttpRedirect const &rhs) {
  (void)rhs;
  return *this;
}

void HttpRedirect::handleRedirect(Request const &req, Response &response,
                                  Configuration const &conf) {
  std::string root = conf.get_value("root")[0];
  if (root.empty())
    root = ".";
  if (*root.rbegin() == '/' && *req.getFilePath().begin() == '/')
    root = root.substr(0, root.size() - 1);
  std::string path = req.getFilePath();
  size_t pos = path.find_first_of("?");
  std::string query_data;
  if (pos != path.npos) {
      query_data = path.substr(pos + 1);
      path = path.substr(0, pos);
  }
  path = root + path;
  std::string redir = conf.get_redirect(req.getFilePath());
  if (!redir.empty()) {
    response._statusCode = 302;
    if (redir.find("http://") == redir.npos)
      redir = "http://" + redir;
    response.setHeader("Location", redir, true);
    return;
  }
  response._path = path;
  if (req.line().getMethod() == "POST" && fileStatus(path) == FILE_DIR)
    return;
  if (req.isCGI())
    return;
  std::vector<Location> lv = conf.get_locations_by_path(req.getFilePath());
  std::vector<std::string> tryFiles;
  if (!lv.empty()) {
    tryFiles = lv[0].get_value("index");
  }
  if (fileStatus(path) == FILE_REG) {
    if (!query_data.empty())
        path = path + query_data;
    response._path = std::string(path);
    return;
  }
  if (fileStatus(path) == FILE_DIR) {
    response._statusCode = 403;
    for (size_t i = 0; i < tryFiles.size(); ++i) {
      std::string filePath(path + tryFiles[i]);
      if (fileStatus(filePath) == FILE_REG) {
        response._statusCode = 200;
        response._path = std::string(filePath);
        }
        return;
      }
  } else if (fileStatus(path) == FILE_NOT) {
    response._statusCode = 404;
  }
}

bool HttpRedirect::isRedirect() { return _redirect; }

std::string const &HttpRedirect::location() { return _location; }
