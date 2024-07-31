#include "HttpAutoindex.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "http_utils.hpp"
#include <algorithm>
#include <ctime>
#include <dirent.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <vector>

HttpAutoindex::HttpAutoindex() {}

HttpAutoindex::~HttpAutoindex() {}

HttpAutoindex::HttpAutoindex(HttpAutoindex const &src) { (void)src; }

HttpAutoindex &HttpAutoindex::operator=(HttpAutoindex const &rhs) {
  (void)rhs;
  return *this;
}

inline std::string do_tabs(size_t size) {
  std::string tmp = "";
  int total = 64 - size;
  while (total > 8) {
    tmp += "\t";
    total -= 8;
  }
  return tmp;
}
inline std::string get_fileinfo(std::string const &filepath) {
  std::ostringstream oss;
  struct stat sb;
  if (stat(filepath.c_str(), &sb) == -1) {
    std::cerr << "Error getting file info for " << filepath << std::endl;
    return "";
  }
  struct tm *time = gmtime(&sb.st_ctime);
  if (!time) {
    std::cerr << "Error getting file info for " << filepath << std::endl;
    return "";
  }

  char timeString[20];
  if (strftime(timeString, sizeof(timeString), "%d-%b-%Y %H:%M", time) == 0) {
    std::cerr << "strftime failed" << std::endl;
    return "";
  }
  std::string date(timeString);
  oss << date;
  oss << std::setw(24) << sb.st_size;
  return oss.str();
}
inline std::string listDirectories(std::string const &filepath,
                                   std::string const &path) {
  std::vector<std::string> v;
  DIR *dirp = opendir(filepath.c_str());

  struct dirent *dp;
  while ((dp = readdir(dirp)) != NULL) {
    v.push_back(dp->d_name);
  }
  closedir(dirp);
  std::ostringstream ss;
  std::string abspath(path);
  if (*abspath.rbegin() != '/')
    abspath += "/";
  std::sort(v.begin(), v.end());
  v.erase(v.begin());
  while (!v.empty()) {
    ss << "<a href=\"" << v.front() << "\">" << v.front();
    if (fileStatus(v.front()) == FILE_DIR)
      ss << "/";
    ss << "</a>";
    ss << do_tabs(v.front().size());
    if (v.front() != "..")
      ss << get_fileinfo(filepath + v.front());
    ss << "\n";
    v.erase(v.begin());
  }
  return ss.str();
}
std::string HttpAutoindex::generateIndex(Request const &req,
                                         std::string const &path) {
  std::ostringstream ss;

  if (*path.rbegin() != '/')
      throw HttpAutoindex::FolderRedirect();
  if (!path.empty()) {
    ss << "<DOCTYPE !html><html><body><h1>Index of " << req.getFilePath()
       << "</h1>";
    ss << "<hr>";
    ss << "<pre>";
    ss << listDirectories(path, req.getAbsPath());
    ss << "</pre><hr>";
    ss << "</body></html>";
  } else {
    throw HttpAutoindex::NoPathException();
  }
  return ss.str();
}

std::string HttpAutoindex::rewriteLocation(std::string const &path){
    return path + "/";
}
const char *HttpAutoindex::NoPathException::what() const throw() {
  return "Provided path could not be Autoindexed";
}
const char *HttpAutoindex::FolderRedirect::what() const throw() {
  return "Handing proper path to browser";
}
