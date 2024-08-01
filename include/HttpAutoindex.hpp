/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpAutoindex.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 18:45:33 by aloubier          #+#    #+#             */
/*   Updated: 2024/08/01 18:45:34 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    static std::string rewriteLocation(std::string const &path);
class NoPathException: public std::exception {
    public:
        const char * what() const throw();
};
class FolderRedirect: public std::exception {
    public:
        const char * what() const throw();
};
private:
};
