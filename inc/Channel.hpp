#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "IRC.hpp"

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
		std::vector<Client>	getClients() { return _clients; }
		std::vector<Client>	getOps() { return _operators; }
		size_t	getClientsSize(){ return _clients.size(); }
		size_t	getOpsSize(){ return _operators.size(); }
		void	setTopic(const std::string& topic) { _topic = topic; }
		void	setKey(const std::string& key) { _key = key; }
		std::string getKey() { return _key; }
		void 	setInviteOnly(bool isInviteOnly) {_isInviteOnly = isInviteOnly;}
		bool	getInviteOnly(){return _isInviteOnly;}
		void 	setrestrictTopic(bool restrictTopic) {_restrictTopic = restrictTopic;}
		bool	getRestrictTopic(){return _restrictTopic;}
		void	setClientLimit(int limit) {_clientLimit = limit;}
		int		getClientLimit() { return _clientLimit; }
		void	addInvitedClient(const std::string &name) {_invitedClients.push_back(name);}
		
		int		mode(const std::string& modeString, const std::string &arg,  Client &client);
		int		mode_get(Client &client);
		int		mode_invite(const std::string& modeString);
		int		mode_topic(const std::string& modeString);
		int		mode_key(const std::string& modeString, const std::string &arg);
		int		mode_op(const std::string& modeString, const std::string& arg, Client &client);
		int		mode_limit(const std::string& modeString, const std::string &arg);
		int		clientMessage(const std::string& message, Client &sender);
		int		channelMessage(const std::string& message);
		int		addOperator(Client& client);
		int		join(Client& client, const std::string& key);
		int		part(Client &client, const std::string &reason);
		int		kick(Client &kicker, const std::string& user, const std::string& reason);
		int		topic(const std::string& newTopic, Client &client);
		int		addClient(Client client);
		std::string	getClientList(void);
		void	removeClient(const std::string &name);
		void	removeOperator(const std::string &name);
		bool	clientIsOp(const std::string &name);
		bool	clientIsInChannel(const std::string &name);
		Client	*findOps(const std::string &name);
		void	removeInvitedClient(const std::string &name);
		bool	clientIsInvited(const std::string &name);
};	

#endif
