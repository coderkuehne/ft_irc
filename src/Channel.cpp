#include "IRC.hpp"

int	Channel::clientMessage(const std::string& message, Client &sender)
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
	int total_clients = _clients.size() + _operators.size();
	int limit = getClientLimit();

	if (limit > 0 && total_clients > limit)
		return (1);
	_operators.push_back(client);
	(void)_server; //temp
	return (0);
}

int	Channel::addClient(Client client)
{
	int total_clients = _clients.size() + _operators.size();
	int limit = getClientLimit();
	if (limit > 0 && total_clients > limit)
		return (1);
	_clients.push_back(client);
	return (0);
}

std::string	Channel::getClientList(void)
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

void	Channel::removeClient(const std::string &name)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (name == _clients[i].getNickname())
		{
			_clients.erase(_clients.begin() + i);
			return ;
		}
	}
	return ;
}

void	Channel::removeOperator(const std::string &name)
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (name == _operators[i].getNickname())
		{
			_operators.erase(_operators.begin() + i);
			return ;
		}
	}
	return ;
}

bool	Channel::clientIsOp(const std::string &name)
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (name == _operators[i].getNickname())
    		return (true);
	}
	return (false);
}

bool	Channel::clientIsInChannel(const std::string &name)
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

Client	*Channel::findOps(const std::string &name)
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i].getNickname() == name)
			return (&_operators[i]);
	}
	return(NULL);
}

void	Channel::removeInvitedClient(const std::string &name)
{
	for (size_t i = 0; i < _invitedClients.size(); i++)
	{
		if (name == _invitedClients[i])
		{
			_invitedClients.erase(_invitedClients.begin() + i);
			return ;
		}
	}
	return ;
}

bool	Channel::clientIsInvited(const std::string &name)
{
	for (size_t i = 0; i < _invitedClients.size(); i++)
	{
		if (name == _invitedClients[i])
    		return (true);
	}
	return (false);
}
