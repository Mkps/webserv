#pragma once
#include <string>

class Response;
class HttpAutoindex {
public:
    HttpAutoindex();
    ~HttpAutoindex();
    HttpAutoindex(HttpAutoindex const &src);
    HttpAutoindex& operator=(HttpAutoindex const &rhs);
    static std::string generateIndex(std::string const &req_path, std::string const &src);
private:
};
