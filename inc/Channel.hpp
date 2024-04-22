#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"

class Server;

class Channel
{
	private:
		Server*		_server;
		std::string	_name;
		std::string	_topic;
		std::string _key;

		std::vector<Client>	_operators;
		std::vector<Client>	_clients;
		std::vector<std::string> _invitedClients;

		bool	_isInviteOnly;
		bool	_restrictTopic;
		int		_clientLimit;

	public:
		Channel(const std::string& name, const std::string& key, Server* server): _server(server), _name(name), _topic("No topic yet"), _key(key), _isInviteOnly(false), _restrictTopic(false), _clientLimit(0){}
		~Channel() {}

		bool	operator==(Channel& channel) { return _name == channel._name; }

		std::string	getName() { return _name; }
		std::string getTopic() { return _topic;}

		void	setTopic(const std::string& topic) { _topic = topic; }
		void	setKey(const std::string& key) { _key = key; }

		void 	setInviteOnly(bool isInviteOnly) { _isInviteOnly = isInviteOnly; }
		bool	getInviteOnly(){ return _isInviteOnly; }
		void 	setrestrictTopic(bool restrictTopic) { _restrictTopic = restrictTopic;}
//		bool	getRestrictTopic(){ return _restrictTopic; }
		void	setClientLimit(int limit) { _clientLimit = limit; }
		int		getClientLimit() { return _clientLimit; }
		void	addInvitedClient(const std::string& name) { _invitedClients.push_back(name);}

		int		clientMessage(const std::string& message, Client& sender);
		int		channelMessage(const std::string& message);

		int		join(Client& client, const std::string& key);
		int		part(Client& client, const std::string& reason);
		int		kick(Client& kicker, const std::string& user, const std::string& reason);

		int		checkMode(Client& client);
		int		topic(const std::string& newTopic, Client& client);

		int		addOperator(Client& client);
		int		addClient(Client& client);

		std::string	getClientList();

		void	removeClient(const std::string& name);
		void	removeOperator(const std::string& name);

		bool	clientIsOp(const std::string& name);
		Client*	findOps(const std::string& name);
		bool	clientIsInChannel(const std::string& name);


		bool	clientIsInvited(const std::string& name);
		void	removeInvitedClient(const std::string& name);

};

#endif
