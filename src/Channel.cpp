#include "IRC.hpp"
#include "Commands.hpp"
#include "Channel.hpp"

// int Channel::kickClient(const std::string channel, const std::string target, Client &client)
// {
//     (void)client;
//     std::cout << "Kicking " << target << "from " << channel << std::endl;
//     return (0);
// }

int	Channel::clientMessage(std::string message, Client &sender)
{
	for (clientIt it = _operators.begin(); it != _operators.end(); ++it) {
		if (*it != sender)
			_server->sendToClient(message, *it);
	}
	for (clientIt it = _clients.begin(); it != _clients.end(); ++it) {
		if (*it != sender)
			_server->sendToClient(message, *it);
	}
	return (0);
}

int	Channel::channelMessage(std::string message)
{
	for (clientIt it = _operators.begin(); it != _operators.end(); ++it)
		_server->sendToClient(message, *it);
	for (clientIt it = _clients.begin(); it != _clients.end(); ++it)
		_server->sendToClient(message, *it);
	return (0);
}

int	Channel::addOperator(Client client) {
	int total_clients = _clients.size() + _operators.size();
	int limit = getClientLimit();

	if (limit > 0 && total_clients > limit)
		return (1);
	_operators.push_back(client);
	(void)_server; //temp
	return (0);
}
