#include "HttpAutoindex.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <sstream>
#include <dirent.h>
#include <vector>
#include <iostream>
#include <algorithm>

HttpAutoindex::HttpAutoindex(){
}

HttpAutoindex::~HttpAutoindex(){
}

HttpAutoindex::HttpAutoindex(HttpAutoindex const &src){
    (void) src;
}

HttpAutoindex& HttpAutoindex::operator=(HttpAutoindex const &rhs){
    (void)rhs;
    return *this;
}

inline std::string listDirectories(std::string const &host, std::string const &req_path, std::string const &path) {
    std::vector<std::string> v;
    DIR* dirp = opendir(path.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        std::cout << dp->d_name << std::endl;
        v.push_back(dp->d_name);
    }
    closedir(dirp);
    std::ostringstream ss;
    std::string filepath = req_path;
    if (*filepath.end() != '/')
        filepath = req_path +"/";
    std::sort(v.begin(), v.end());
    while (!v.empty()) {
        ss << "<p><a href=\"http://"<< host << filepath << v.front() << "\">" << v.front() << "</a></p>\n";
        v.erase(v.begin());
    }
    return ss.str();
}
std::string HttpAutoindex::generateIndex(Request const &req, std::string const &path){
    std::ostringstream ss; 
    std::string host = "";
    if (req.headers().find("host") != req.headers().end())
        host = req.headers().find("host")->second;
    else 
        throw std::runtime_error("No host set");
    std::string filepath = req.getFilePath();
    host = req.headers().find("host")->second;
    if (path.empty())
        ss << "<DOCTYPE !html><html><body>Index not found." << "</body></html>";
    else {
        ss << "<DOCTYPE !html><html><body><h1>Index of " << path << "</h1>";
        ss << listDirectories(host, filepath, path);
        ss << "</body></html>";
    }
    return ss.str();
}
