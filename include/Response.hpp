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
		std::string	_path;
		size_t		_bytes_sent;
		size_t		_offset;
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
		void				setBodyError(int status);
		void				setBody(std::string const & filename);
		hashmap const &		getHeader() const;
		std::string const &	getHeaderValue(std::string const & key) const;
		void				setHeader(std::string const & key, std::string const & value);
		void				setDefaultHeaders();

		void				clear();
		std::string			writeHeader();
		void				processRequest(Request const & req);
		void				sendResponse(int clientSocket);
		std::string			chunkResponse();

		int                 handleCGI(int clientSocket, std::string const & script, std::string const & query);
		void				httpMethodDelete(Request const& req);
		void				httpMethodGet(Request const& req);
		void				httpMethodPost(Request const& req);
		std::string			findContentType();
};
#endif
