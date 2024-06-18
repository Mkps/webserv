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

class Request;
class Response {
	private:
		short		_responseCode;
		std::string	_contentType;
		std::string	_body;
		size_t		_contentLength;
	public:
		Response();
		Response(Request const &request);
		~Response();
		Response(Response const & src);
		Response& operator= (Response const & rhs);

		short const &		getResponseCode() const;
		std::string const &	getContentType() const;
		size_t const &		getContentLength() const;
		std::string			getResponseMsg();
		void				setBodyError();
};
#endif
