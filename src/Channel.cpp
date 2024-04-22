#include "IRC.hpp"
#include "Commands.hpp"

int	Channel::checkMode(Client& client)
{
	std::string	modeString = "";
	std::string	modeArgs = "";
	if (_isInviteOnly)
		modeString += "i";
	if (_restrictTopic)
		modeString += "t";
	if (_clientLimit > 0) {
		modeString += "l";
		std::stringstream ss;
		ss << _clientLimit;
		modeArgs += " " + ss.str();
	}
	if (!_key.empty()) {
		modeString += "k";
		modeArgs += " " + _key;
	}
	if (!modeString.empty())
		modeString = "+" + modeString;

	std::string	message = modeString + modeArgs;
	_server->sendToClient(buildReply(_name, client.getNickname(), 324, message, 1, _name.c_str()), client);
	return (0);
}

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
	int total_clients = _clients.size() + _operators.size();
	int limit = getClientLimit();

	if (limit > 0 && total_clients > limit)
		return (1);
	_operators.push_back(client);
	(void)_server; //temp
	return (0);
}
