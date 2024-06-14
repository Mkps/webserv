#include <netinet/in.h>
#include <string>
class ASocket {
	public:
		ASocket(std::string IPAddress, int portNumber);
		~ASocket();
		
		void		startListen();
		void		log(std::string const & msg) const;
		int const &	getFd() const;
	private:
		ASocket();

		int			_socket;
		sockaddr_in _socketAddr;

};
