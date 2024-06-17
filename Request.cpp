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

void	Request::retrieveHeaders(std::string const & request)
{
}
Request::Request(std::string const &request)
	: _requestLine(request)
{
	if (_requestLine.isRequestLineValid() == false)
		std::cerr << "invalid method" << std::endl;
	else
		std::cout << "method ok" << std::endl;
}

Request::~Request()
{
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
