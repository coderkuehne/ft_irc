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
		int		addOperator(Client client);	
		int		clientMessage(std::string message, Client &sender);
		int		channelMessage(std::string message);

		int		mode(const std::string& modeString, const std::string &arg,  Client &client);
		int		mode_get(Client &client);
		int		mode_invite(const std::string& modeString, const std::string &arg,  Client &client);
		int		mode_topic(const std::string& modeString, const std::string &arg,  Client &client);
		int		mode_key(const std::string& modeString, const std::string &arg,  Client &client);
		int		mode_op(const std::string& modeString, const std::string& arg, Client &client);
		int		mode_limit(const std::string& modeString, const std::string &arg,  Client &client);

		int	addClient(Client client)
		{
			int total_clients = _clients.size() + _operators.size();
			int limit = getClientLimit();

			if (limit > 0 && total_clients > limit)
				return (1);
			_clients.push_back(client);
			return (0);
		}

		std::string	getClientList(void) {
			std::string	list = ":";
			for (clientIt it = _operators.begin(); it != _operators.end(); ++it) {
				if (!list.empty())
					list += " ";
				list += "@" + it->getNickname();
			}
			for (clientIt it = _clients.begin(); it != _clients.end(); ++it) {
				if (!list.empty())
					list += " ";
				list += it->getNickname();
			}
			return list;
		}

		void	removeClient(const std::string &name)
		{
			for (size_t i = 0; i < _clients.size(); i++)
			{
				if (name == _clients[i].getNickname())
				{
					_clients.erase(_clients.begin() + i);
					if (DEBUG)
						std::cout << "removed " << _clients[i].getNickname() << " from " << _name << std::endl;
					return ;
				}
			}
			return ;
		}

		void	removeOperator(const std::string &name)
		{
			for (size_t i = 0; i < _operators.size(); i++)
			{
				if (name == _operators[i].getNickname())
				{
					_operators.erase(_operators.begin() + i);
					if (DEBUG)
						std::cout << "removed " << _operators[i].getNickname() << " from " << _name << std::endl;
					return ;
				}
			}
			return ;
		}

		bool	clientIsOp(const std::string &name)
		{
    		for (size_t i = 0; i < _operators.size(); i++)
			{
        		if (name == _operators[i].getNickname())
            		return (true);
			}
    		return (false);
    	}

		bool	clientIsInChannel(const std::string &name)
		{
			if (!clientIsOp(name))
			{
				for (size_t i = 0; i < _clients.size(); i++)
				{
					if (name == _clients[i].getNickname())
						return (true);
				}
			}
			else
			{
				for (size_t i = 0; i < _operators.size(); i++)
				{
					if (name == _operators[i].getNickname())
						return (true);
				}
			}
			return (false);
		}

		Client	*findOps(const std::string &name)
		{
			for (size_t i = 0; i < _operators.size(); i++)
			{
				if (_operators[i].getNickname() == name)
					return (&_operators[i]);
			}
			return(NULL);
		}

		void	removeInvitedClient(const std::string &name)
		{
			for (size_t i = 0; i < _invitedClients.size(); i++)
			{
				if (name == _invitedClients[i])
				{
					_invitedClients.erase(_invitedClients.begin() + i);
					if (DEBUG)
						std::cout << "removed " << _invitedClients[i] << " from " << _name << std::endl;
					return ;
				}
			}
			return ;
		}

		bool	clientIsInvited(const std::string &name)
		{
    		for (size_t i = 0; i < _invitedClients.size(); i++)
			{
        		if (name == _invitedClients[i])
            		return (true);
			}
    		return (false);
    	}
};	

#endif
