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
		struct addrinfo		_hints;
		struct addrinfo*	_serverInfo;

		std::vector<Client>			_clients;
		std::vector<struct pollfd>	_fds;
		static bool					_running;

	public:
		Server(const std::string& port = "6789", const std::string& password = "123");
		~Server(void);

		void	start(void);
		void	setHints(void);
		int		createSocket(void);
		int		acceptSocket(void);
		int		sendSocket(std::string message, int clientSocket);
		int		receiveSocket(int clientSocket);
		void	closeSocket(void);
		int		verifyPassword(int clientSocket, std::string password);

		static void	signalHandler(int signum);
};

#endif
