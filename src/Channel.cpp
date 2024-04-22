#include "IRC.hpp"
#include "Commands.hpp"

int	Channel::clientMessage(const std::string& message, Client& sender)
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

int	Channel::channelMessage(const std::string& message)
{
	for (clientIt it = _operators.begin(); it != _operators.end(); ++it)
		_server->sendToClient(message, *it);
	for (clientIt it = _clients.begin(); it != _clients.end(); ++it)
		_server->sendToClient(message, *it);
	return (0);
}

int	Channel::addOperator(Client& client) {
	size_t	total_clients = _clients.size() + _operators.size();
	size_t	limit = getClientLimit();

	if (limit > 0 && total_clients > limit)
		return (1);
	_operators.push_back(client);
	return (0);
}

int	Channel::addClient(Client& client)
{
	size_t	total_clients = _clients.size() + _operators.size();
	size_t	limit = getClientLimit();

	if (limit > 0 && total_clients > limit)
		return (1);
	_clients.push_back(client);
	return (0);
}

std::string	Channel::getClientList()
{
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
