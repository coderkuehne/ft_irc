#include "IRC.hpp"
#include "Channel.hpp"
#include "Commands.hpp"

int	Channel::join(Client& client, const std::string& key) {
	if (_isInviteOnly && clientIsInvited(client.getNickname()))
		removeInvitedClient(client.getNickname());
	else if (_isInviteOnly)
		return (_server->sendToClient(buildReply(SERVER, client.getNickname(), 473, "", 1, _name.c_str()), client));
	else if (!_key.empty() && key != _key)
		return (_server->sendToClient(buildReply(SERVER, client.getNickname(), 475, "", 1, _name.c_str()), client));

	if (getClientList().size() == 1)
		addOperator(client);
	else if (addClient(client)) {
		return (_server->sendToClient(buildReply(SERVER, client.getNickname(), 471, "", 1, _name.c_str()), client));
	}
	channelMessage(buildReply(client.getNickname(), _name, JOIN, "", 0));
	_server->sendToClient(buildReply(SERVER, client.getNickname(), 332, _topic, 1, _name.c_str()), client);
	checkMode(client);
	return 0;
}

int	Channel::part(Client &client, const std::string &reason)
{
	std::string	name = client.getNickname();

	if (!clientIsInChannel(name))
		return _server->sendToClient(buildReply(SERVER, client.getNickname(), 442, "", 0), client);
	if (reason != "QUITTER")
		channelMessage(buildReply(client.getNickname(), _name, PART, reason, 0));
	if (clientIsOp(name))
		removeOperator(name);
	else
		removeClient(name);
	return 0;
}
