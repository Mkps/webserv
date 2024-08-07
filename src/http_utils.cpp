/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http_utils.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 18:44:09 by aloubier          #+#    #+#             */
/*   Updated: 2024/08/01 18:44:09 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http_utils.hpp"
#include "Client.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <sys/socket.h>
#include <sys/stat.h>

std::string to_hex(size_t value) {
  std::ostringstream oss(std::ios::binary);
  oss << std::hex << value;
  return oss.str();
}

int sendStr(int clientSocket, std::string const &str) {
  return send(clientSocket, str.c_str(), str.size(), 0);
}

int sendChunk(int const &clientSocket, std::string const &chunk,
              size_t &offset) {
  int tmp = 0;
  int ret = 0;
  std::string size = to_hex(chunk.size()) + "\r\n";
  std::string chunkMsg = chunk + "\r\n";
  int sent = 0;
  while (sent < static_cast<int>(size.size())) {
    tmp = send(clientSocket, size.c_str(), size.size(), 0);
    if ((size_t)tmp != size.size()) {
      std::cerr << "Warning: Size chunk was not fully sent" << std::endl;
      offset -= (size.size() - 2 - tmp);
    }
    if (tmp < 0)
      return tmp;
    sent += tmp;
  }
  ret += sent;
  sent = 0;
  while (sent < static_cast<int>(chunkMsg.size())) {
    tmp = send(clientSocket, chunkMsg.c_str(), chunkMsg.size(), 0);
    if ((size_t)tmp != size.size()) {
      std::cerr << "Warning: Chunk was not fully sent" << std::endl;
      offset -= (chunkMsg.size() - 2 - tmp);
    }
    if (tmp < 0)
      return tmp;
    sent += tmp;
  }
  ret += sent;
  return ret;
}

int fileStatus(const std::string &path) {
  struct stat buf;
  if (path.empty())
    return FILE_NOT;
  if (stat(path.c_str(), &buf) == 0) {
    if (buf.st_mode & S_IFDIR || *path.rbegin() == '/')
      return FILE_DIR;
    else if (buf.st_mode & S_IFREG)
      return FILE_REG;
    else
      return FILE_ELS;
  } else
    return FILE_NOT;
}

int getFileSize(const std::string &path) {
  struct stat buf;
  if (stat(path.c_str(), &buf) == 0)
    return buf.st_size;
  return FILE_NOT;
}

template <typename T> std::string NumberToString(T Number) {
  std::ostringstream ss;
  ss << Number;
  return ss.str();
}

std::string sizeToStr(std::string::size_type size) {
  std::ostringstream ss;
  ss << size;
  return ss.str();
}

typedef std::map<std::string, std::string> hashmap;
void showHeaders(hashmap tmp) {
  for (hashmap::const_iterator it = tmp.begin(); it != tmp.end(); ++it) {
    std::cout << "key " << it->first << " value " << it->second << std::endl;
  }
}

std::string get_current_date() {
  time_t now = time(0);
  struct tm tm;
  char buf[100];

  gmtime_r(&now, &tm);
  strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &tm);

  return std::string(buf);
}

void logStep(std::string const &s, int n) {
  std::cerr << s << " ";
  for (int i = 0; i < n; ++i) {
    std::cerr << "#";
  }
  std::cerr << std::endl;
};

void logItem(std::string const &s, std::string const &item) {
  std::cerr << s << " " << item << std::endl;
};
void logItem(std::string const &s, int const &item) {
  std::cerr << s << " " << item << std::endl;
};
int logError(std::string const &s, int const &status) {
  std::cerr << "Log: " << s << std::endl;
  return status;
}
std::string errPage(Client &client, size_t error_code) {
  return client.getConfig().get_error_page(error_code);
}

std::string getResponse(short status) {
  if (status >= 100 && status < 200) {
    return "Informational";
  } else if (status >= 200 && status < 300) {
    if (status == 201)
      return "Created";
    else if (status == 204)
      return "No Response";
    return "OK";
  } else if (status >= 300 && status < 400) {
    if (status == 301)
      return "Moved Permanently";
    else if (status == 302)
      return "Found";
    return "Redirection";
  } else if (status >= 400 && status < 500) {
    if (status == 400)
      return "Bad Request";
    else if (status == 403)
      return "Forbidden";
    else if (status == 404)
      return "Not Found";
    else if (status == 405)
      return "Method Not Allowed";
    else if (status == 406)
      return "Not Acceptable";
    else if (status == 411)
      return "Length Required";
    else if (status == 414)
      return "URI Too Long";
    else
      return "Client Error";
  } else if (status >= 500 && status < 600) {
    if (status == 502)
      return "Bad Gateway";
    if (status == 504)
      return "Gateway Timeout";
    if (status == 505)
      return "HTTP Version Not Supported";
    return "Internal Server Error";
  } else
    return "Unidentified Error";
}
static void trim(std::string &s) {
  size_t first = s.find_first_not_of(' ');
  size_t start = s.find_last_not_of(' ');
  s = s.substr(first, start - first + 1);
}

// trim from both ends (copying)
std::string trim_copy(std::string s) {
  trim(s);
  return s;
}

std::string formatHeader(const std::string &input) {
  std::string result = input;
  bool capitalizeNext = true;
  for (std::string::iterator it = result.begin(); it != result.end(); ++it) {
    if (!std::isalpha(*it)) {
      capitalizeNext = true;
    } else if (capitalizeNext && std::isalpha(*it)) {
      *it = std::toupper(*it);
      capitalizeNext = false;
    } else {
      *it = std::tolower(*it);
    }
  }
  return result;
}
