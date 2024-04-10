#ifndef CLIENT_HPP
# define CLIENT_HPP
#include "Server.hpp"

class Client {
	private:
		bool		_authenticated;
		bool		_registered;
		int			_socket;
//		std::string	_ip;
		std::string	_nick;
		std::string	_username;

	public:
		Client(int socket): _socket(socket), _nick(""), _username("") { _authenticated = false; _registered = false; };
//		Client(int socket, struct sockaddr_in addr) { _socket = socket; _ip = inet_ntoa(addr.sin_addr); };
		~Client() {};

		int			getSocket() { return _socket; };
		std::string	getNickname() { return _nick; };
		void		setNickname(const std::string& nick) { _nick = nick; };
		std::string	getUsername() { return _username; };
		void		setUsername(const std::string& username) { _username = username; };
		bool		isRegistered() { return _registered; };
		void		beRegistered(void) { _registered = true; };
		//		void setIp(std::string ip) { _ip = ip; };
//		std::string getIp() { return _ip; };

};

#endif
