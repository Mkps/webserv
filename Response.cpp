/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 15:30:25 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/18 15:30:26 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Request.hpp"
#include <sstream>
Response::Response()
{
	_responseCode = 400;
	_contentType = "text/html";
	_contentLength = 12;
	_body = "<!DOCTYPE html>\n<html>\n<head>\n</head>\n<body>\n<h3>NO</h3>\n</body>\n</html>\r\n";
	
}

Response::Response(Request const &request)
{
	(void)request;
}

Response::~Response()
{
}

Response::Response(Response const & src)
{
	*this = src;
}

Response& Response::operator= (Response const & rhs)
{
	_responseCode = rhs.getResponseCode();
	_contentType = rhs.getContentType();
	_contentLength = rhs.getContentLength();
	return *this;
}

short const &		Response::getResponseCode() const
{
	return _responseCode;
}

std::string const &	Response::getContentType() const
{
	return _contentType;
}

size_t const &			Response::getContentLength() const
{
	return _contentLength;
}

void				Response::setBodyError()
{

	std::ostringstream s;
	s << "<!DOCTYPE html>\n<html>\n<head>\n</head>\n<body>\n<hl>" << _responseCode << "</h1>\n<p>Error</p></body>\n</html>\r\n";
	_body = s.str();
}

std::string		Response::getResponseMsg()
{
	std::ostringstream s;
	s << "HTTP/1.1 " << _responseCode << " KO\n" << "content-type " << _contentType\
	<< "\n"	<< " content-length " << _contentLength \
	<< "\n\n";
	if (_responseCode != 200)
		setBodyError();
	s << _body;
	return s.str();
}

