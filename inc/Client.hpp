#ifndef CLIENT_HPP
# define CLIENT_HPP
#include "Server.hpp"

class Client {
	private:
		int			_socket;
		std::string	_ip;
		std::string	_nick;
		std::string	_username;

	public:
		Client() { _socket = 0; _ip = ""; };
		Client(int socket, struct sockaddr_in addr) { _socket = socket; _ip = inet_ntoa(addr.sin_addr); };
		~Client() {};

		void setSocket(int socket) { _socket = socket; };
		int getSocket() { return _socket; };
		void setIp(std::string ip) { _ip = ip; };
		std::string getIp() { return _ip; };

};

#endif
