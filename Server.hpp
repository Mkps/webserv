#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>

class Server {
	public:
		Server(std::string config);
		~Server();

	private:
		Server();
		Server(Server const & src);
		Server & operator= (Server const & rhs);
};
#endif //SERVER_HPP
