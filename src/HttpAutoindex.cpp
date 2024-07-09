#include "HttpAutoindex.hpp"
#include "Response.hpp"
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

inline std::string listDirectories(std::string const &req_path, std::string const &path) {
    std::vector<std::string> v;
    DIR* dirp = opendir(path.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        std::cout << dp->d_name << std::endl;
        v.push_back(dp->d_name);
    }
    closedir(dirp);
    std::ostringstream ss;
    std::sort(v.begin(), v.end());
    while (!v.empty()) { //need to find a way to get the current hostname + port
        ss << "<p><a href=\"http://localhost:8000/" << req_path << v.front() << "\">" << v.front() << "</a></p>\n";
        v.erase(v.begin());
    }
    return ss.str();
}
std::string HttpAutoindex::generateIndex(std::string const &req_path, std::string const &path){
    std::ostringstream ss; 
    if (path.empty())
        ss << "<DOCTYPE !html><html><body>Index not found." << "</body></html>";
    else {
        ss << "<DOCTYPE !html><html><body><h1>Index of " << path << "</h1>";
        ss << listDirectories(req_path, path);
        ss << "</body></html>";
    }
    return ss.str();
}
