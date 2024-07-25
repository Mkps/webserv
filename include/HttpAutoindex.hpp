#pragma once
#include <string>

class Response;
class Request;
class HttpAutoindex {
public:
    HttpAutoindex();
    ~HttpAutoindex();
    HttpAutoindex(HttpAutoindex const &src);
    HttpAutoindex& operator=(HttpAutoindex const &rhs);
    static std::string generateIndex(Request const &req, std::string const &src);
class NoPathException: public std::exception {
    public:
        const char * what() const throw();
};
private:
};
