#ifndef SERVER_HPP
# define SERVER_HPP

# include "IRC.hpp"

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
		~Server();

		static void	signalHandler(int signum);

		void	start();
		void	setHints();
		int		createSocket();
		int		acceptSocket();
		int		sendToClient(const std::string&, const Client&) const;
		int		receiveFromClient(Client&);
		void	closeSockets();

		int		authenticatePassword(Client&, std::string&);
		Client*	usernameIsRegistered(const std::string&);
		int		changeNickname(const std::string& nick, Client& client);
		int		setUsername(std::string& user, Client& client);
		void	registerClient(Client& client) const;

		void	parseCommand(const std::string&, Client&);

		Client*		findClient(const std::string&);
		Channel*	findChannel(const std::string&);
		void		addChannel(Channel& channel){ _channels.push_back(channel); };
		int			removeChannel(Channel& channel);
		int			clearEmptyChannels();

		int		joinChannel(std::string&, std::string&, Client&);
		void	names(Client& client, std::string& channelName);
		int		channelTopic(const std::string& channel,const std::string& newTopic, Client& client);
		int		kickClient(const std::string& channelName, const std::string& target, const std::string& reason, Client& client);
		int		inviteChannel(const std::string& _target, const std::string& _channel, const Client& client);
		int		partChannel(const std::string&, const std::string&, Client&);
		int		quit(Client& client, std::string& quitMessage);

		int		sendMessage(std::string&, std::string&, Client& client);
		int		ChannelMessage(std::string&, std::string&, Client& client);

};

#endif
