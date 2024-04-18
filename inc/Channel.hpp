#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"

class Channel
{
	private:
		std::string	_name;
		std::string	_topic;
		std::string _key;
		std::vector<Client>	_operators;
		std::vector<Client>	_clients;
		bool	_isInviteOnly;

	public:
		Channel(std::string name, std::string key): _name(name), _key(key), _isInviteOnly(false) {
			_topic = "No topic yet";
		};
		~Channel() {};

		std::string	getName() { return _name; };
		std::string getTopic() { return _topic;};
		std::vector<Client>	getClients() { return _clients; };
		std::vector<Client>	getOps() { return _operators; };
		size_t	getClientsSize(){ return _clients.size(); }
		size_t	getOpsSize(){ return _operators.size(); }
		void	setTopic(const std::string& topic) { _topic = topic; };
		void	setKey(const std::string& key) { _key = key; };
		void	addClient(Client client) { _clients.push_back(client); };
		void	addOperator(Client client) { _operators.push_back(client); };
		bool	getIsInviteOnly(){return (_isInviteOnly);};


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
		};

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
};	

#endif
