#ifndef SERVER_HPP
# define SERVER_HPP

# include "IRC.hpp"
# include "Channel.hpp"
# include "Client.hpp"

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
		std::vector<Channel>		_channels;
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
		int		sendToChannel(std::string);
		int		receiveFromClient(Client);
		void	closeSocket(void);
		int		verifyPassword(int clientSocket, std::string password);
		void    parseCommand(std::string command, Client &client);

		Client*		getClient(const std::string&);
		Channel*	getChannel(const std::string&);

		static void	signalHandler(int signum);

		int cmd_nick(std::string nick, Client &client);
		int cmd_msg(std::vector<std::string> args, Client &client);
		int cmd_join(std::vector<std::string> args);
		int cmd_leave(std::vector<std::string> args);
		
};

#endif
