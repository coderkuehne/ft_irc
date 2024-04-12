#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"

class Channel
{
	private:
		std::string	_name;
		std::string	_topic;
		std::string _key;
		std::vector<Client>	_clients;

	public:
		Channel(std::string name, std::string key): _name(name), _key(key) {
			_topic = "No topic yet";
		};
		~Channel() {};

		std::string	getName() { return _name; };
		std::string getTopic() { return _topic;};
		std::vector<Client>	getClients() { return _clients; };
		void	setKey(const std::string& key) { _key = key; };
		void	addClient(Client client) { _clients.push_back(client); };
		void	removeClient(Client client) { (void)client; return ; };
};

#endif
