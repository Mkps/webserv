/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 18:43:51 by aloubier          #+#    #+#             */
/*   Updated: 2024/08/01 18:43:54 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpMethod.hpp"
#include "CgiHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "http_utils.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>

void HttpMethod::del(Request const &req, Response &res) {
  (void)req;
  if (res._statusCode > 400)
    return;
  if (remove(res._path.c_str()) == EXIT_SUCCESS)
    res._statusCode = 204;
  else
    res._statusCode = 403;
}

inline std::string cutQuery(std::string const &path) {

  size_t query_pos = path.find("?");
  if (query_pos != std::string::npos) {
    return path.substr(0, query_pos);
  } else
    return path;
}

void HttpMethod::get(Request const &req, Response &res) {
  if (res._statusCode == 200 && req.isCGI()) {
    CgiHandler cgi(res._path);
    res._cgi = cgi;
    res._cgi.setHeaders(req.headers());
    res._cgi.setCgiBin(req.getCgiPath());
    res._statusCode = res._cgi.handleGet();
    if (res._statusCode == 200)
      res._body = res._cgi.body();
  }
  if (!req.isCGI() && res._statusCode == 200) {
    if (fileStatus(res._path) == FILE_REG &&
        !access(res._path.c_str(), F_OK | R_OK))
      res.setBody(res._path);
    else
      res._statusCode = 403;
  }
}

inline std::string generate_filename() {
  std::ostringstream oss;
  time_t t = time(NULL);
  oss << "resource_" << t;
  return oss.str();
}

inline std::string extractBoundary(const std::string &contentType) {
  std::string boundaryPrefix = "boundary=";
  size_t boundaryPos = contentType.find(boundaryPrefix);
  if (boundaryPos != std::string::npos) {
    return "--" + contentType.substr(boundaryPos + boundaryPrefix.size());
  }
  return "";
}

inline std::vector<std::string> splitParts(const std::string &body,
                                           const std::string &boundary) {
  std::vector<std::string> parts;
  size_t pos = 0;
  size_t end;
  if (boundary.empty()) {
    parts.push_back(body);
    return parts;
  }
  while ((end = body.find(boundary, pos)) != std::string::npos) {
    parts.push_back(body.substr(pos, end - pos));
    pos = end + boundary.size();
  }
  if (pos < body.size()) {
    parts.push_back(body.substr(pos));
  }
  return parts;
}

inline std::string extractFileName(const std::string &part) {
  std::string filename;
  std::istringstream partStream(part);
  std::string line;
  size_t pos = part.find("Content-Disposition:");
  if (pos != std::string::npos) {
    size_t filenamePos = part.find("filename=", pos);
    if (filenamePos != std::string::npos) {
      size_t start = part.find('"', filenamePos) + 1;
      size_t end = part.find('"', start);
      filename = part.substr(start, end - start);
    }
  }
  return filename;
}

inline std::string extractContent(const std::string &part) {
  size_t headerEnd = part.find("\r\n\r\n");
  if (headerEnd != std::string::npos) {
    return part.substr(headerEnd + 4);
  }
  return "";
}

inline std::string extractFormData(const std::string &part) {
  size_t headerEnd = part.find("\r\n\r\n");
  if (headerEnd != std::string::npos) {
    std::cerr << "part" << part << "\npart sub" << part.substr(headerEnd + 4)
              << std::endl;
    return part.substr(headerEnd + 4);
  }
  return "";
}

inline std::vector<std::string> get_multipart(const Request &req) {
  std::string requestBody = req.body();
  std::string contentType = req.findValue("content-type");
  if (contentType.find("multipart") == contentType.npos)
    return std::vector<std::string>();
  std::string boundary = extractBoundary(contentType);
  std::vector<std::string> parts = splitParts(requestBody, boundary);
  return parts;
}

void HttpMethod::post(Request const &req, Response &res,
                      std::string const &upload) {
  if (req.isCGI()) {
    CgiHandler cgi(res._path);
    res._cgi = cgi;
    res._cgi.setHeaders(req.headers());
    res._cgi.setRequestBody(req.body());
    res._cgi.setCgiBin(req.getCgiPath());
    int ret = res._cgi.handlePost();
    if (ret == 200)
      res._body = cgi.body();
  } else if (res._statusCode == 200) {
    if (fileStatus(req.getFilePath()) == FILE_REG) {
        std::cout << req.getFilePath() << std::endl;
        get(req, res);
        return ;
    }
    std::string uploadBody = "";
    std::string fileName = "";
    std::ofstream outFile;
    std::string upload_path;
    if (!upload.empty())
      upload_path = upload;
    else
      upload_path = res._path;
    if (fileStatus(upload_path) == FILE_DIR) {
      if (*upload_path.rbegin() != '/')
        upload_path += "/";
      std::vector<std::string> multipart = get_multipart(req);
      if (!multipart.empty()) {
        for (size_t i = 0; i < multipart.size(); ++i) {
          if (fileName.empty()) {
            fileName = extractFileName(multipart[i]);
          }
          if (uploadBody.empty())
            uploadBody = extractContent(multipart[i]);
        }
      } else {
        upload_path += generate_filename();
        uploadBody = req.body();
      }
    }
    if (!fileName.empty())
      upload_path += fileName;
    bool isCreated = false;
    if (access(upload_path.c_str(), F_OK | W_OK)) {
      isCreated = true;
    }
    outFile.open(upload_path.c_str(),
                 std::ios::out | std::ios::binary | std::ios::trunc);
    if (!outFile.good()) {
      res._statusCode = 403;
      return;
    }
    outFile.write(uploadBody.c_str(), uploadBody.size());
    std::string fileAbsPath = req.getAbsPath() + "/" + fileName;
    res.setHeader("Location", fileAbsPath, true);
    if (isCreated) {
      res._statusCode = 201;
    } else {
      res._statusCode = 204;
    }
    return;
  }
}
