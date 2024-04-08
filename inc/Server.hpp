#ifndef SERVER_HPP
# define SERVER_HPP

# include "IRC.hpp"

class Client;

class Server
{
	private:
		const std::string&	_port;
		const std::string	_password;

		int					_socket;

		std::vector<Client>			_clients;
		std::vector<struct pollfd>	_fds;
		static bool					_running;

	public:
		Server(const std::string& = "6789", const std::string& password = "123");
		~Server();

		void	start();
		int		createSocket();
		int		acceptSocket();
		int		sendSocket(std::string message, int client_socket);
		int		receiveSocket(int client_socket);
		void	closeSocket();
		int		verifyPassword(int client_socket, std::string password);

		static void	signalHandler(int signum);
};

#endif
