#ifndef SERVER_HPP
# define SERVER_HPP

# include "IRC.hpp"

class Client;

class Server
{
	typedef std::vector<Client>::iterator	clientIt;
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
		int		sendToClient(std::string, Client);
		int		receiveFromClient(Client);
		void	closeSocket(void);

//		void	checkPassword(std::string data);
//		void	parseOther(std::string client_data);
		Client*	checkClientRegistered(const std::string&);

		bool	authenticatePassword(Client&, std::string&);
		bool	registerClientNames(Client&, std::string&);
		void	authenticateClient(Client&, std::string&);

		Client*	getClient(const std::string&);

		static void	signalHandler(int signum);
};

#endif
