#ifndef SERVER_HPP
# define SERVER_HPP

# include "IRC.hpp"
# include "Channel.hpp"
# include "Client.hpp"

class Server
{
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

		int	guestCount;

	public:
		Server(const std::string& port = "7779", const std::string& password = "123");
		~Server(void);

		void	start(void);
		void	setHints(void);
		int		createSocket(void);
		int		acceptSocket(void);
		int		sendToClient(const std::string&, const Client&) const;
		int		sendToChannel(std::string, Channel &channel, Client &client);
		int		receiveFromClient(Client&);
		void	closeSockets(void);

		Client*	checkClientRegistered(const std::string&);

		void	parseCommand(const std::string&, Client&);

		int	authenticatePassword(Client&, std::string&);
//		bool	registerClientNames(Client&, std::string&);
//		void	authenticateClient(Client&, std::string&);

		Client*		findClient(const std::string&);
		Channel*	findChannel(const std::string&);
		int			removeChannel(Channel& channel);

		static void	signalHandler(int signum);

		int		changeNickname(const std::string& nick, Client &client);
		int		setUsername(std::string& user, Client &client);
		void	registerClient(Client& client) const;

		int		sendMessage(std::string&, std::string&, Client &client);
		int		ChannelMessage(std::string&, std::string&, Client &client);
		int		joinChannel(std::string&, std::string&, Client&);
		int		partChannel(const std::string&, const std::string&, Client&);
		int		cmd_leave(std::vector<std::string> args);
		int		quit(Client &client, std::string& quitMessage);
		void	addChannel(Channel channel){ _channels.push_back(channel); };
		void	printClients(void);

		void	names(Client& client, std::string& channelName);

		int		mode(const std::string&, const std::string&, const std::string& ,Client &client);

		int		topic(const std::string& channel,const std::string& newTopic, Client &client);
		
		int		kickClient(const std::string &channelName, const std::string &target, const std::string& reason, Client &client);
		int		inviteChannel(const std::string &_target, const std::string &_channel, const Client client);
};

#endif
