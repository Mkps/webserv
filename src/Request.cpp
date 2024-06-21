/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 14:51:09 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/17 14:51:09 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

#include <iostream>
#include <sstream>


Request::~Request()
{
}

Request::Request()
{
}

Request::Request(std::string const &request)
	: _requestLine(request)
{
	if (_requestLine.isRequestLineValid() == false)
		std::cerr << "invalid method" << std::endl;
	fetchHeaders(request);
}

Request::Request(Request const &src)
{
	*this = src;
}

Request& Request::operator= (Request const &rhs)
{
	_requestLine = rhs.getRequestLine();
	_requestHeaders = rhs.getRequestHeaders();
	return *this;
}

//Will eventually need to take into account the host provided by config
std::string	Request::getFilePath() const
{
	std::ostringstream s;
	s << _requestLine.getRequestUri();
	return s.str();
}

void	Request::setRequest(std::string const & request)
{
	RequestLine req(request);
	_requestLine = req;
	if (_requestLine.isRequestLineValid() < 0)
		std::cerr << "invalid method or http version" << std::endl;
	fetchHeaders(request);
}

Request const &Request::getRequest() const
{
	return *this;
}

RequestLine const &Request::getRequestLine() const
{
	return _requestLine;
}

hashmap const &Request::getRequestHeaders() const
{
	return _requestHeaders;
}

inline void trim(std::string &s) {
	size_t first = s.find_first_not_of(' ');
	size_t last = s.find_last_not_of(' ');
	s = s.substr(first, last - first + 1);
}

// trim from both ends (copying)
inline std::string trim_copy(std::string s) {
	trim(s);
	return s;
}

void	Request::fetchHeaders(std::string const & request)
{
	std::istringstream req(request);
	std::string header;
	std::string::size_type index;
	while (std::getline(req, header) && header[0] != '\r') {
		index = header.find(':', 0);
		if(index != std::string::npos) {
			_requestHeaders.insert(std::make_pair(
						trim_copy(header.substr(0, index)), 
						trim_copy(header.substr(index + 1))
						));
		}
	}
}

