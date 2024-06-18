/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 15:30:28 by aloubier          #+#    #+#             */
/*   Updated: 2024/06/18 15:30:29 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include <map>

typedef std::map<std::string, std::string> hashmap;
class Request;
class Response {
	private:
		short		_statusCode;
		hashmap		_responseHeaders;
		std::string	_body;
	public:
		Response();
		Response(Request const &request);
		~Response();
		Response(Response const & src);
		Response& operator= (Response const & rhs);

		void				setStatusCode(short status);
		short const &		getStatusCode() const;
		std::string const &	getContentType() const;
		size_t const &		getContentLength() const;

		std::string			getResponseMsg();
		void				setBodyError();
		hashmap const &		getHeader() const;
		std::string const &	getHeaderValue(std::string const & key) const;
		void				setHeader(std::string const & key, std::string const & value);

		std::string			writeHeader();
		void				send() const;
};
#endif
