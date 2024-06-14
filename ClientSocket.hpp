#include <netinet/in.h>
#include <string>
#include "Socket.hpp"

class ClientSocket : public Socket{
	public:
		ClientSocket(std::string IPAddress, int portNumber);
		~ClientSocket();
		
		void		startListen();
		void		log(std::string const & msg) const;
		int const &	getFd() const;
	private:
		ClientSocket();

		int			_socket;
		sockaddr_in _socketAddr;
};
