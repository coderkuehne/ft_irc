#include "Server.hpp"
#include "Parser.hpp"
#include "Client.hpp"
#include "Commands.hpp"

int	Server::authenticatePassword(Client& client, std::string& inputPassword) {
	if (inputPassword.empty())
			return sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "PASS"), client);
	if (inputPassword == _password) {
		client.beAuthenticated();
		return 0;
	}
	else
		return sendToClient(buildReply(SERVER, client.getNickname(), 464, "", 0), client);
}

int	Server::changeNickname(const std::string& nick, Client &client)
{
	if (nick.empty())
	{
		std::cerr << RED << "No nickname given" << RESET << std::endl;
		return sendToClient(buildReply(SERVER, "*", 431, "", 0), client);
	}
	if (nick[0] == '#' || nick[0] == ':' || nick[0] == ' ')
	{
		std::cerr << RED << "Invalid nickname" << RESET << std::endl;
		return sendToClient(buildReply(SERVER, "*", 432, "", 1, nick.c_str()), client);
	}
	if (findClient(nick)) {
		std::cerr << RED << "Nickname already in use" << RESET << std::endl;
		sendToClient(buildReply(SERVER, "*", 433, "", 1, nick.c_str()), client);
		return (-1);
	}
	client.setNickname(nick);
	sendToClient(buildReply(SERVER, client.getNickname(), NICK, "", 1, nick.c_str()), client);
	registerClient(client);
	return (0);
}

int	Server::setUsername(std::string& user, Client &client)
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
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (user == _clients[i].getUsername())
		{
			std::cerr << RED << "Username already in use" << RESET << std::endl;
			sendToClient(buildReply(SERVER, client.getNickname(), NOTICE, ":Username is taken, registering as guest", 1, client.getNickname().c_str()), client);
			std::stringstream	ss;
			ss << ++guestCount;
			user = "Guest" + ss.str();
		}
	}
	client.setUsername(user);
	registerClient(client);
	return (0);
}

void	Server::registerClient(Client &client) const {
	if (!client.isRegistered() && !client.getNickname().empty() && !client.getUsername().empty()) {
		client.beRegistered();
		std::cout << "New user registered: Nickname: " << client.getNickname() << " Username: " << client.getUsername() << std::endl;
		sendToClient(buildReply(SERVER, client.getNickname(), 001, "", 1, client.getNickname().c_str()), client);
	}
}

int Server::ChannelMessage(std::string& target, std::string& message, Client &client)
{
	if (target.empty() || message.empty())
	{
		std::cerr << RED << "Invalid command" << RESET << std::endl;
		sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "PRIVMSG"), client);
		return (1);
	}
	
//	Channel	*channel = getChannel(target);

	Channel	*channel = findChannel(target);
	if (!channel)
	{
		std::cerr << RED << "Invalid target" << RESET << std::endl;
		sendToClient(buildReply(SERVER, client.getNickname(), 403, "", 1, target.c_str()), client);
		return (1);
	}
//	sendToChannel(":" + client.getNickname() + " " + message + END, *channel, client);
	sendToChannel(buildReply(client.getNickname(), channel->getName(), PRIVMSG, message, 0), *channel, client);
	return (0);
}

int Server::sendMessage(std::string& target, std::string& message, Client &client)
{
	if (target.empty() || message.empty())
	{
		std::cerr << RED << "Invalid command" << RESET << std::endl;
//		sendToClient(":ft_irc 461 * :Not enough parameters" + END, client);
		sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "PRIVMSG"), client);
		return 1;
	}

	if (target[0] == '#') {
		ChannelMessage(target, message, client);
		return 1;
	}

	Client	*recipient = findClient(target);
	if (!recipient)
	{
		std::cerr << RED << "Invalid target" << RESET << std::endl;
//		sendToClient(":ft_irc 401 * :No such user" + END, client);
		sendToClient(buildReply(SERVER, client.getNickname(), 401, "", 1, target.c_str()), client);
		return 1;
	}
	sendToClient(buildReply(client.getNickname(), recipient->getNickname(), PRIVMSG, message, 0), *recipient);
	return 0;
}

void Server::responseForClientJoiningChannel(Client &client, Channel &channel)
{
//	sendToClient(":" + client.getNickname() + " JOIN " + channel.getName() + END, client);
	sendToClient(buildReply(client.getNickname(), channel.getName().c_str(), JOIN, "", 0), client);
	sendToClient(":ft_irc 332 " + client.getNickname() + " " + channel.getName() + " :" + channel.getTopic() + END, client);
}

void	Server::names(Client& client, std::string& channelName)
{
	Channel	*channel = findChannel(channelName);
	if (!channel)
	{
//		sendToClient(":ft_irc 401 * :No such channel" + END, client);
		sendToClient(buildReply(SERVER, client.getNickname(), 403, "", 1, channelName.c_str()), client);
		return;
	}
//	std::string	list = channel->getClientList();
//	sendToClient(":ft_irc 353 " + client.getNickname() + " = " + channelName + " :" + channel->getClientList() + END, client);
	sendToClient(buildReply(SERVER, client.getNickname(), 353, "", 3, "=", channelName.c_str(), channel->getClientList().c_str()), client);
//	sendToClient(":ft_irc 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list" + END, client);
	sendToClient(buildReply(SERVER, client.getNickname(), 366, "", 1, channelName.c_str()), client);
//	sendToClient(":ft_irc 352 " + client.getNickname() + " " + channel->getName() + " " + list + END, client);
//	sendToClient(":ft_irc 315 " + client.getNickname() + " " + channel->getName() + " :End of /WHO list" + END, client);
}

int Server::joinChannel(std::string& channelName, std::string& key, Client &client)
{
	std::cout << "what is input " << channelName << " ." << std::endl;
	if (channelName.empty())
	{
//		sendToClient(":ft_irc 461 *" + client.getNickname() + " " + "JOIN" + " :Not enough parameters" + END, client);
		sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "JOIN"), client);
		return (1);
	}
	if (channelName[0] != '#')
	{
		sendToClient(":ft_irc 476 " + channelName + " :Bad Channel Mask, Put '#' Before Channel Name" + END, client);
		return (1);
	}

	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (channelName == _channels[i].getName())
		{
			_channels[i].addClient(client);
			responseForClientJoiningChannel(client, _channels[i]);
//			sendToChannel(":" + client.getNickname() + " JOIN " + _channels[i].getName() + END, _channels[i], client);
			sendToChannel(buildReply(client.getNickname(), _channels[i].getName(), JOIN, "", 0), _channels[i], client);
			return (0);
		}
	}
	Channel newChannel(channelName, key); //if no key is supplied, key is set to ""
	newChannel.addOperator(client);
	addChannel(newChannel);

	std::cout << "hiello?" << std::endl;
	responseForClientJoiningChannel(client, newChannel);

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
	if (messageCode == 001)
		reply += recipient;
	reply += END;
	return reply;
}

int Server::quit(Client &client, std::string& quitMessage)
{
	for (clientIt it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (*it == client)
		{
			//part from all channels

			close(client.getSocket());
			_fds.erase(_fds.begin() + std::distance(_clients.begin(), it) + 1);
			_clients.erase(it);
			std::cout << "Successful quit" << std::endl;
			break;
		}
	}
	for (size_t i = 0; i < _clients.size(); ++i) {
//		sendToClient(":" + client.getNickname() + " QUIT :Quit " + quitMessage + END, _clients[i]);
		sendToClient(buildReply(client.getNickname(), ":Quit", QUIT, quitMessage, 0), _clients[i]);
	}
	return (0);
}

int Server::cmdTopic(const std::string& _channel,const std::string& newTopic, Client &client)
{
	std::string name = client.getNickname();
	std::cout <<"this is "<< newTopic << std::endl;
	if (_channel.empty())
	{
		sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "TOPIC"), client);

		return (1);
	}
	Channel *channel = findChannel(_channel);

	if (channel == NULL)
	{
		sendToClient(buildReply(SERVER, client.getNickname(), 403, "", 1, _channel.c_str()), client);
		return (1);
	}
	if (!channel->clientIsInChannel(name))
	{
		sendToClient(buildReply(SERVER, client.getNickname(), 442, "", 1, _channel.c_str()), client);
		return (1);
	}
	bool isOP = channel->clientIsOp(name);

	if (newTopic.empty())
	{
		sendToClient(buildReply(SERVER, client.getNickname(), 332, "", 2, _channel.c_str(), channel->getTopic().c_str()), client);
		return (1);
	}
	if (isOP)
	{
		channel->setTopic(newTopic);
		std::time_t currTime = std::time(NULL);
		std::stringstream notSS;
    	notSS << currTime;
		std::string currTimestamp = notSS.str();
		for (size_t i = 0; i < channel->getOpsSize(); i++)
		{
			sendToChannel(buildReply(SERVER, channel->getOps()[i].getNickname(), 332, "", 2, channel->getName().c_str(), channel->getTopic().c_str()), *channel, channel->getOps()[i]);
			sendToChannel(buildReply(SERVER, channel->getOps()[i].getNickname(), 333, "", 3, channel->getName().c_str(), name.c_str(), currTimestamp.c_str()), *channel, channel->getOps()[i]);

		}
		for (size_t i = 0; i < channel->getClientsSize(); i++)
		{
			sendToChannel(buildReply(SERVER, channel->getClients()[i].getNickname(), 332, "", 2, channel->getName().c_str(), channel->getTopic().c_str()), *channel, channel->getClients()[i]);
			sendToChannel(buildReply(SERVER, channel->getClients()[i].getNickname(), 333, "", 3, channel->getName().c_str(), name.c_str(), currTimestamp.c_str()), *channel, channel->getClients()[i]);
		}
	}
	if (!isOP)
	{
		sendToClient(buildReply(SERVER, client.getNickname(), 482, "", 1, _channel.c_str()), client);
		return (1);
	}
	return (0);
}

void Server::printClients(void)
{
	std::cout << "Total Clients:" << _clients.size() << std::endl;
	for (size_t i = 0; i < _clients.size(); i++)
	{
		std::cout << "\tClient " << i << ": " << _clients[i].getNickname() << " Username : " << _clients[i].getUsername() << "on Socket :" << _clients[i].getSocket() << std::endl;
	}
}

int Server::kickClient(const std::string &_channel,const std::string &_target, Client &client)
{
	Channel *channel = findChannel(_channel);
	std::string name = client.getNickname();

	std::cout << "Kick user " << _target << std::endl;
	if (channel == NULL)
	{
		sendToClient(":ft_irc NOTICE " + name + ": No such channel" + END, client);
		return (0);
	}
	if (!channel->clientIsOp(name))
	{
		sendToClient(":ft_irc 481 " + name + " " + _channel + " :You`re not a channel operator" + END, client);
		return (0);
	}
	if (!channel->clientIsInChannel(_target))
	{
		sendToClient(":ft_irc 441 " + name + " " + _target + " " + _channel + ":User not on channel" + END, client);
		return (0);
	}
	sendToClient(":" + name + " KICK " + _channel + " " + _target + END, client);
	sendToChannel(":" + name + " KICK " + _channel + " " + _target + END, *channel , client);
	if (channel->clientIsOp(_target))
		channel->removeOperator(_target);
	channel->removeClient(_target);
	return (1);
}

int Server::partChannel(const std::string &_channel, const std::string &_reason, Client &client)
{
	Channel *channel = findChannel(_channel);
	std::string name = client.getNickname();

	if (channel == NULL)
	{
		sendToClient(":ft_irc NOTICE " + client.getNickname() + " " + _channel + " :No such channel" + END, client);
		return (0);
	}
	sendToClient(":" + name + " PART " + _channel + " " +_reason + END, client);
	sendToChannel(":" + name + " PART " + _channel + " " + _reason + END, *channel , client);
	channel->removeClient(name);
	if (channel->clientIsOp(name))
		channel->removeOperator(name);
	return (1);
}

int Server::inviteChannel(const std::string &_target, const std::string &_channel, const Client client)
{
	Channel *channel = findChannel(_channel);
	std::string name = client.getNickname();

	if (channel == NULL)
	{
		sendToClient(":ft_irc 403 " + name + " " + _channel + " :No such channel" + END, client);
		return (0);
	}
	// if (channel->clientIsInChannel(name))
	// {
	// 	sendToClient(":ft_irc 441 " + name + " " + _channel + " " + ":They aren`t on that Channel" + END, client);
	// 	return (0);
	// }
	if (!channel->clientIsOp(name) && channel->getIsInviteOnly())
	{
		sendToClient(":ft_irc 481 " + name + " " + channel->getName() + " :You`re not a channel operator" + END, client);
		return (0);
	}
	sendToClient(":ft_irc 341", client); // confirmation message to sender
	sendToClient(":" + name + " INVITE "+ _target + " " + _channel + END, *findClient(_target)); // invite to target user
	return (1);
}
