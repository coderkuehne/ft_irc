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
		int		sendToClient(std::string, Client&);
		int		sendToChannel(std::string, Client&);
		int		receiveFromClient(Client&);
		void	closeSocket(void);

		void	parseCommand(std::string command, Client &client);

		Client*	checkClientRegistered(const std::string&);

		bool	authenticatePassword(Client&, std::string&);
		bool	registerClientNames(Client&, std::string&);
		void	authenticateClient(Client&, std::string&);

		Client*		getClient(const std::string&);
		Channel*	getChannel(const std::string&);

		static void	signalHandler(int signum);

		int cmd_nick(std::string nick, Client &client);
		int cmd_msg(std::vector<std::string> args,size_t msg_size, Client &client);
		int cmd_join(std::vector<std::string> args);
		int cmd_leave(std::vector<std::string> args);
		
};

#endif
