#include <netinet/in.h>
#include <string>

#include "ASocket.hpp"
class ClientSocket;
class ServerSocket : public ASocket{
	public:
		ServerSocket(std::string IPAddress, int portNumber);
		~ServerSocket();
		
		void		startListen();
		void		log(std::string const & msg) const;
		int const &	getFd() const;
	private:
		ServerSocket();

		int			_socket;
		sockaddr_in _socketAddr;
		ClientSocket* clientArray;

};
