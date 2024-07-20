#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include "Request.hpp"
#include "Response.hpp"
#include "Configuration.hpp"

class Socket;

#define BUFFER_SIZE 1024

enum e_client_state {
	CLIENT_CONNECTED,
	CLIENT_DISCONNECTED
};

class Client {
	public:
//		Client(Socket *socket, size_t id);
		Client(Socket *socket, size_t id, Configuration _config);
		~Client(void);

		Socket		*getSocket() const;
		std::string	getIp() const;
		size_t		getId() const;
		int			getFd() const;
		std::string	getRequest() const;
        Configuration const &getConfig() const;
		void		clearRequest(void);

		int			recvRequest();
		void		handleResponse();
		void		log(void) const;

	private:
		Socket		*_socket;
		Response	_res;
		Request		_req;
		std::string	_ip;
		size_t		_id;
        Configuration _config;
		int			_fd;
		std::string	_request;
};

std::ostream & operator<<(std::ostream & o, Client const & r);

#endif
