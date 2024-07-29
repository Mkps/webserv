#pragma once

#include <string>

class Request;
class Response;
class HttpMethod {
    public:
static void del(Request const &req, Response &res);
static void get(Request const &req, Response &res);
static void post(Request const &req, Response &res, std::string const &path);
};
