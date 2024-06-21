#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Socket;

#define BUFFER_SIZE 1024

enum e_client_state {
	CLIENT_CONNECTED,
	CLIENT_DISCONNECTED
};

class Client {
	public:
		Client(Socket *socket, size_t id);
		~Client(void);

		int			getFd() const;
		std::string	getRequest() const;
		std::string	getIp() const;

		int			recvRequest();
		void		log(void) const;

	private:
		Socket		*_socket;
		std::string	_ip;
		size_t		_id;
		int			_fd;
		std::string	_request;
};

std::ostream & operator<<(std::ostream & o, Client const & r);

#endif