#include "IRC.hpp"

int	Server::authenticatePassword(Client& client, std::string& inputPassword) {
	if (inputPassword.empty())
			return (sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "PASS"), client));
	if (inputPassword == _password) {
		client.beAuthenticated();
		return (0);
	}
	else
		return (sendToClient(buildReply(SERVER, client.getNickname(), 464, "", 0), client));
}

int	Server::changeNickname(const std::string& nick, Client& client)
{
	if (!client.getNickname().empty())
		return (0);
	if (nick.empty())
	{
		std::cerr << RED << "No nickname given" << RESET << std::endl;
		return (sendToClient(buildReply(SERVER, "*", 431, "", 0), client));
	}
	if (nick[0] == '#' || nick[0] == ':' || nick[0] == ' ')
	{
		std::cerr << RED << "Invalid nickname" << RESET << std::endl;
		return (sendToClient(buildReply(SERVER, "*", 432, "", 1, nick.c_str()), client));
	}
	if (findClient(nick)) {
		std::cerr << RED << "Nickname already in use" << RESET << std::endl;
		sendToClient(buildReply(SERVER, "*", 433, "", 1, nick.c_str()), client);
		return (-1);
	}
	sendToClient(buildReply(client.getNickname(), nick, NICK, "", 0), client);
	client.setNickname(nick);
	registerClient(client);
	return (0);
}

int	Server::setUsername(std::string& user, Client& client)
{
	if (client.isRegistered()) {
		std::cerr << RED << "User already registered" << RESET << std::endl;
		sendToClient(buildReply(SERVER, client.getNickname(), 462, "", 0), client);
		return (-1);
	}
	if (user.empty())
	{
		std::cerr << RED << "No username given" << RESET << std::endl;
		sendToClient(buildReply(SERVER, "*", 431, "", 0), client);
		return (-1);
	}
	if (usernameIsRegistered(user)) {
		std::cerr << RED << "Username already in use" << RESET << std::endl;
		sendToClient(buildReply(SERVER, client.getNickname(), NOTICE, ":Username is taken, registering as guest", 1, client.getNickname().c_str()), client);
		std::stringstream	ss;
		ss << ++guestCount;
		user = "Guest" + ss.str();
	}
	client.setUsername(user);
	registerClient(client);
	return (0);
}

void	Server::registerClient(Client& client) const {
	if (!client.isRegistered() && !client.getNickname().empty() && !client.getUsername().empty()) {
		client.beRegistered();
		std::cout << "New user registered: Nickname: " << client.getNickname() << " Username: " << client.getUsername() << std::endl;
		sendToClient(buildReply(SERVER, client.getNickname(), 001, "", 0), client);
	}
}

int	Server::ChannelMessage(std::string& target, std::string& message, Client& client)
{
	Channel	*channel = findChannel(target);

	if (!channel)
	{
		std::cerr << RED << "Invalid target" << RESET << std::endl;
		sendToClient(buildReply(SERVER, client.getNickname(), 403, "", 1, target.c_str()), client);
		return (1);
	}
	return (channel->clientMessage(buildReply(client.getNickname(), channel->getName(), PRIVMSG, message, 0), client));
}

int	Server::sendMessage(std::string& target, std::string& message, Client& client)
{
	if (target.empty() || message.empty())
	{
		std::cerr << RED << "Invalid command" << RESET << std::endl;
		sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "PRIVMSG"), client);
		return (1);
	}
	if (target[0] == '#') {
		ChannelMessage(target, message, client);
		return (1);
	}
	Client	*recipient = findClient(target);

	if (!recipient)
	{
		std::cerr << RED << "Invalid target" << RESET << std::endl;
		sendToClient(buildReply(SERVER, client.getNickname(), 401, "", 1, target.c_str()), client);
		return (1);
	}
	if (recipient->getNickname() == "ChatGPT")
		return (_bot->parseBotCommand("Hello who are you?", client));
	return (sendToClient(buildReply(client.getNickname(), recipient->getNickname(), PRIVMSG, message, 0), *recipient));
}

void	Server::names(Client& client, std::string& channelName)
{
	Channel	*channel = findChannel(channelName);
	if (!channel)
	{
		sendToClient(buildReply(SERVER, client.getNickname(), 403, "", 1, channelName.c_str()), client);
		return;
	}
	sendToClient(buildReply(SERVER, client.getNickname(), 353, "", 3, "=", channelName.c_str(), channel->getClientList().c_str()), client);
	sendToClient(buildReply(SERVER, client.getNickname(), 366, "", 1, channelName.c_str()), client);
}

int	Server::joinChannel(std::string& channelName, std::string& key, Client& client)
{
	if (channelName.empty())
		return (sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "JOIN"), client));
	if (channelName[0] != '#')
		return (sendToClient(buildReply(SERVER, client.getNickname(), 476, "", 0), client));
	Channel*	channel = findChannel(channelName);
	if (!channel) {
		Channel newChannel(channelName, key, this);
		newChannel.join(client, key);
		addChannel(newChannel);
	}
	else
		channel->join(client, key);
	return (0);
}

std::string	buildReply(const std::string& sender, const std::string& recipient, int messageCode, const std::string& message, int paramCount, ...) {
	std::string	reply = ":" + sender + " ";
	reply += macroToCommand(messageCode) + " ";
	if (recipient.empty())
		reply +=  "* ";
	else
		reply += recipient + " ";
	va_list	args;
	va_start(args, paramCount);

	for (int i = 0; i < paramCount; ++i)
		reply += std::string(va_arg(args, char *)) + " ";
	if ((messageCode >= 1 && messageCode <= 5) || messageCode > 400 || messageCode == 366) // pre-defined: 1-5 are welcome, 400+ are errors, 366 is end of NAMES list
		reply += NUMERIC_REPLIES.at(messageCode);
	else if (!message.empty())
		reply += message;
	else if (messageCode == KICK)
		reply += "You have been kicked";
	if (messageCode == 001)
		reply += recipient;
	reply += END;
	return reply;
}

int	Server::quit(Client& client, std::string& quitMessage)
{
	std::string	nickname = client.getNickname();
	for (channelIt it = _channels.begin(); it != _channels.end(); ++it) {
		if ((*it).clientIsInChannel(nickname)) {
			if ((*it).part(client, "QUITTER") == 2)
				_channels.erase(it);
		}
	}
	for (clientIt it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (*it == client)
		{
			close(client.getSocket());
			_fds.erase(_fds.begin() + std::distance(_clients.begin(), it) + 1);
			_clients.erase(it);
			std::cout << nickname << " has quit" << std::endl;
			break;
		}
	}
	for (clientIt it = _clients.begin(); it != _clients.end(); ++it)
		sendToClient(buildReply(nickname, ":Quit", QUIT, quitMessage, 0), *it);
	return (0);
}

int	Server::channelTopic(const std::string& channelName,const std::string& newTopic, Client& client)
{
	std::string	name = client.getNickname();
	std::cout <<"this is "<< newTopic << std::endl;
	if (channelName.empty())
		return (sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "PRIVMSG"), client));

	Channel	*channel = findChannel(channelName);
	if (!channel)
		return(sendToClient(buildReply(SERVER, client.getNickname(), 403, "", 0), client));
	channel->topic(newTopic, client);
	return (0);
}

int	Server::kickClient(const std::string& channelName,const std::string& target, const std::string& reason, Client& client)
{
	Channel		*channel = findChannel(channelName);
	std::string	name = client.getNickname();

	if (target.empty() || channelName.empty())
		return (sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 0), client));
	if (!channel)
		return (sendToClient(buildReply(SERVER, client.getNickname(), 403, "", 0), client));
	if (channel->kick(client, target, reason) == 2)
		removeChannel(*channel);
	return (1);
}

int	Server::partChannel(const std::string& channelName, const std::string& reason, Client& client)
{
	Channel	*channel = findChannel(channelName);

	if (!channel)
		return (sendToClient(buildReply(SERVER, client.getNickname(), 403, "", 0), client));
	if (channel->part(client, reason) == 2)
		removeChannel(*channel);
	return (0);
}

int	Server::removeChannel(Channel& channel)
{
	for (channelIt it = _channels.begin(); it != _channels.end(); ++it) {
		if (*it == channel)
			_channels.erase(it);
	}
	return (0);
}

int	Server::inviteChannel(const std::string& _target, const std::string& _channel, const Client& client)
{
	Channel		*channel = findChannel(_channel);
	std::string	name = client.getNickname();

	if (_target.empty() || _channel.empty())
		return (sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "PRIVMSG"), client));
	if (!channel)
		return(sendToClient(buildReply(SERVER, client.getNickname(), 442, "", 1, _channel.c_str()), client));
	if (!findClient(_target))
		return (sendToClient(buildReply(SERVER, name, 401, "", 1, _target.c_str()), client));
	if (!channel->clientIsInChannel(name))
		return(sendToClient(buildReply(SERVER, client.getNickname(), 441, "", 1, _target.c_str()), client));
	if (channel->clientIsInChannel(_target))
		return(sendToClient(buildReply(SERVER, client.getNickname(), 443, "", 2, _target.c_str(), _channel.c_str()), client));
	if (!channel->clientIsOp(name) && channel->getInviteOnly())
		return(sendToClient(buildReply(SERVER, client.getNickname(), 482, "", 1, _channel.c_str()), client));
	channel->addInvitedClient(_target);
	sendToClient(buildReply(SERVER, client.getNickname(), 341, "", 2, _target.c_str(), _channel.c_str()), client);
	sendToClient(buildReply(SERVER, client.getNickname(), INVITE, "", 2, _target.c_str(), _channel.c_str()), *findClient(_target));
	return (1);
}
