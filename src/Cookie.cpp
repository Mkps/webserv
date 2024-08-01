/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 18:46:13 by aloubier          #+#    #+#             */
/*   Updated: 2024/07/02 18:46:14 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"

Cookie::Cookie(){
}

Cookie::Cookie(std::string const &data){
    import(data);
}

Cookie::~Cookie(){
}

Cookie::Cookie(Cookie const &src){
    (void)src;
}

Cookie &Cookie::operator=(Cookie const &rhs){
    (void)rhs;
    return *this;
}

bool Cookie::empty() const {
    return _data.empty();
}
//#include <iostream>
//returns the full cookieheader value as a string
std::string Cookie::full() const {
    std::string tmp;
    tmp = "";
    hashmap::const_iterator it = _data.begin();
    for (; it != _data.end(); ++it) {
        tmp += it->first + "=" + it->second + "; ";
    }
    size_t pos = tmp.find_last_of(";");
    if (pos != tmp.npos)
        tmp = tmp.substr(0, pos);
    //std::cout << "Cookies are " << tmp << std::endl;
    return tmp;
}

void Cookie::import(std::string const &data){
    size_t pos = 0;
    size_t end;
    while ((end = data.find(';', pos)) != std::string::npos) {
        std::string cookie = data.substr(pos, end - pos);
        size_t eqPos = cookie.find('=');
        if (eqPos != std::string::npos) {
            std::string name = cookie.substr(0, eqPos);
            std::string value = cookie.substr(eqPos + 1);
            _data[name] = value;
        }
        pos = end + 1;
    }
    if (pos < data.size()) {
        std::string cookie = data.substr(pos);
        size_t eqPos = cookie.find('=');
        if (eqPos != std::string::npos) {
            std::string name = cookie.substr(0, eqPos);
            std::string value = cookie.substr(eqPos + 1);
            _data[name] = value;
        }
    }
}

void Cookie::insert(std::string const &key, std::string const &value) {
    if (!key.empty() && !value.empty())
        _data[key] = value;
}
std::string Cookie::find(std::string const &key) const{
    hashmap::const_iterator it = _data.find(key);
    if (it == _data.end())
        return std::string();
    return it->second;
}

bool Cookie::exist(std::string const &key) const{
    hashmap::const_iterator it = _data.find(key);
    if (it == _data.end())
        return false;
    return true;
}

void Cookie::clear(){
    _data.clear();
}

#include <iostream>
void Cookie::log() const {
    hashmap::const_iterator it = _data.begin();
    for (; it != _data.end(); ++it) {
        std::cout << "Cookie element " << it->first << " value " << it->second << std::endl;
    }

}

void Cookie::setSession(hashmap &session) const {
    hashmap::const_iterator it = _data.begin();
    for (; it != _data.end(); ++it) {
       session[it->first] = it->second;
    }
}
