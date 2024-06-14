#include <netinet/in.h>
#include <string>
class Socket {
	public:
		Socket(std::string IPAddress, int portNumber);
		~Socket();
		
		void	startListen();
		void	log(std::string const & msg) const;
	private:
		Socket();

		int			_socket;
		sockaddr_in _socketAddr;

};
