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
		Server(const std::string& port = "7779", const std::string& password = "123");
		~Server(void);

		void	start(void);
		void	setHints(void);
		int		createSocket(void);
		int		acceptSocket(void);
		int		sendToClient(std::string, Client&);
		int		sendToChannel(std::string, Channel &channel);
		int		receiveFromClient(Client&);
		void	closeSocket(void);

		Client*	checkClientRegistered(const std::string&);

		void	parseCommand(std::string, Client&);

		int	authenticatePassword(Client&, std::string&);
//		bool	registerClientNames(Client&, std::string&);
//		void	authenticateClient(Client&, std::string&);

		Client*		getClient(const std::string&);
		Channel*	getChannel(const std::string&);

		static void	signalHandler(int signum);

		int		changeNickname(std::string nick, Client &client);
		int		setUsername(std::string user, Client &client);
		void	registerClient(Client& client);

		int		cmd_msg(std::vector<std::string> args,size_t msg_size, Client &client);
		int		joinChannel(std::string&, std::string&, Client&);
		int		cmd_leave(std::vector<std::string> args);
		int		cmd_quit(Client &client);
		void	addChannel(Channel channel){ _channels.push_back(channel); };
		void	printClients(void);

		void	welcomePrompt(Client &client, Channel &channel);
		
};

#endif
