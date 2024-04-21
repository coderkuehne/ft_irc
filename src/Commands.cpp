#include "Server.hpp"
#include "Parser.hpp"
#include "Client.hpp"
#include "Commands.hpp"

//user + MODE + channel + arg
//MODE RPL_CREATIONTIME (329)

int	Server::mode(const std::string& channelName, const std::string& modeString, const std::string &arg,  Client &client)
{
	std::string name = client.getNickname();

	if(channelName.empty())
		return (sendToClient(buildReply(SERVER, name, 461, "", 1, "PRIVMSG"), client));
	Channel *channel = findChannel(channelName);

	if (channel == NULL)
		return (sendToClient(buildReply(SERVER, name, 403, "", 1, channelName.c_str()), client));
	if (modeString.empty())
		return (channel->checkMode(client));
	if (!channel->clientIsOp(name))
		return(sendToClient(buildReply(SERVER, client.getNickname(), 482, "", 1, channelName.c_str()), client));
	if (modeString == "-i")
	{
		channel->setInviteOnly(false);
		sendToClient(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()), client);
		sendToChannel(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()),*channel, client);
	}
	if (modeString == "+i")
	{
		channel->setInviteOnly(true);
		sendToClient(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()), client);
		sendToChannel(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()),*channel, client);
	}
	if (modeString == "-t")
	{
		channel->setrestrictTopic(false);
		sendToClient(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()), client);
		sendToChannel(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()),*channel, client);
	}
	if (modeString == "+t")
	{
		channel->setrestrictTopic(true);
		sendToClient(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()), client);
		sendToChannel(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()),*channel, client);
	}
	if (modeString == "-k")
	{
		channel->setKey("");
		sendToClient(buildReply(name, channelName.c_str(), MODE, "", 1, modeString.c_str()), client);
	}
	if (!arg.empty())
	{
		if (modeString == "+o")
		{
			if (!channel->clientIsOp(arg))
			{
				if (!findClient(arg))
					return (sendToClient(buildReply(SERVER, client.getNickname(), 401, "", 1, arg.c_str()), client)); //wrong error 
				channel->addOperator(*findClient(arg));
				channel->removeClient(arg);
				sendToClient(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()), client);
				sendToChannel(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()),*channel, client);
			}
		}
		if (modeString == "-o")
		{
			if (channel->clientIsOp(arg))
			{
				if (!channel->findOps(arg))
					return (sendToClient(buildReply(SERVER, client.getNickname(), 401, "", 1, arg.c_str()), client)); //wrong error
				channel->removeOperator(arg);
				channel->addClient(*findClient(arg));
				sendToClient(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()), client);
				sendToChannel(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()),*channel, client);
			}
		}
		if (modeString == "+l")
		{
			channel->setClientLimit(atoi(arg.c_str()));
			sendToClient(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()), client);
		}
		if (modeString == "-l")
		{
			channel->setClientLimit(0);
			sendToClient(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()), client);
		}
		if (modeString == "+k")
		{
			channel->setKey(arg);
			sendToClient(buildReply(name, channelName.c_str(), MODE, "", 2, modeString.c_str(), arg.c_str()), client);
		}
	}
	return (0);
}

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
//	if (target.empty() || message.empty())
//	{
//		std::cerr << RED << "Invalid command" << RESET << std::endl;
//		sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "PRIVMSG"), client);
//		return (1);
//	}
	//^ already checked in calling function
	Channel	*channel = findChannel(target);

	if (!channel)
	{
		std::cerr << RED << "Invalid target" << RESET << std::endl;
		sendToClient(buildReply(SERVER, client.getNickname(), 403, "", 1, target.c_str()), client);
		return (1);
	}
//	return (sendToChannel(buildReply(client.getNickname(), channel->getName(), PRIVMSG, message, 0), *channel, client));
	return (channel->clientMessage(buildReply(client.getNickname(), channel->getName(), PRIVMSG, message, 0), client));
}

int Server::sendMessage(std::string& target, std::string& message, Client &client)
{
	if (target.empty() || message.empty())
	{
		std::cerr << RED << "Invalid command" << RESET << std::endl;
		sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "PRIVMSG"), client);
		return (1);
	}
	if (target[0] == '#') {
		ChannelMessage(target, message, client);
		return 1;
	}
	Client	*recipient = findClient(target);

	if (!recipient)
	{
		std::cerr << RED << "Invalid target" << RESET << std::endl;
		sendToClient(buildReply(SERVER, client.getNickname(), 401, "", 1, target.c_str()), client);
		return (1);
	}
	return (sendToClient(buildReply(client.getNickname(), recipient->getNickname(), PRIVMSG, message, 0), *recipient));
}

void Server::responseForClientJoiningChannel(Client &client, Channel &channel)
{
	sendToClient(buildReply(client.getNickname(), channel.getName().c_str(), JOIN, "", 0), client);
	sendToClient(":ft_irc 332 " + client.getNickname() + " " + channel.getName() + " " + channel.getTopic() + END, client);
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

int Server::joinChannel(std::string& channelName, std::string& key, Client &client)
{
	if (channelName.empty())
	{
		sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "JOIN"), client);
		return (1);
	}
	if (channelName[0] != '#')
	{
		sendToClient(":ft_irc 476 " + channelName + " :Bad Channel Mask, Put '#' Before Channel Name" + END, client);
		return (1);
	}
	Channel *channel = findChannel(channelName);

	if (channel && !channel->getKey().empty() && key != channel->getKey())
	{
		sendToClient(buildReply(SERVER, client.getNickname(), 475, "", 1, channelName.c_str()), client);
		return (1);
	}
	if (channel)
	{
		bool isInv = channel->getInviteOnly();
		if (isInv && channel->clientIsInvited(client.getNickname()))
		{
			channel->removeInvitedClient(client.getNickname());
			if (channel->addClient(client))
				return (sendToClient(buildReply(SERVER, client.getNickname(), 471, "", 1, channelName.c_str()), client));
			responseForClientJoiningChannel(client, *channel);
			return (sendToChannel(buildReply(client.getNickname(), channel->getName(), JOIN, "", 0), *channel, client));
		}
		else if (isInv)
			return (sendToClient(buildReply(SERVER, client.getNickname(), 473, "", 1, channelName.c_str()), client));

		if (channel->addClient(client))
			return (sendToClient(buildReply(SERVER, client.getNickname(), 471, "", 1, channelName.c_str()), client));
		responseForClientJoiningChannel(client, *channel);
		return (sendToChannel(buildReply(client.getNickname(), channel->getName(), JOIN, "", 0), *channel, client));
	}
	Channel newChannel(channelName, key, this); //if no key key = ""

	newChannel.addOperator(client);
	addChannel(newChannel);
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
	for (size_t i = 0; i < _clients.size(); ++i) 
		sendToClient(buildReply(client.getNickname(), ":Quit", QUIT, quitMessage, 0), _clients[i]);
	return (0);
}

int Server::cmdTopic(const std::string& _channel,const std::string& newTopic, Client &client)
{
	std::string name = client.getNickname();
	std::cout <<"this is "<< newTopic << std::endl;
	if (_channel.empty())
		return (sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "PRIVMSG"), client));
	Channel *channel = findChannel(_channel);

	if (channel == NULL)
		return(sendToClient(buildReply(SERVER, client.getNickname(), 403, "", 1, _channel.c_str()), client));
	if (!channel->clientIsInChannel(name))
		return(sendToClient(buildReply(SERVER, client.getNickname(), 442, "", 1, _channel.c_str()), client));
	if (newTopic.empty())
		return (sendToClient(buildReply(SERVER, client.getNickname(), 332, "", 2, _channel.c_str(), channel->getTopic().c_str()), client));
	bool isOP = channel->clientIsOp(name);

	if (isOP || !channel->getRestrictTopic())
	{
		channel->setTopic(newTopic);
		std::time_t currTime = std::time(NULL);
		std::stringstream notSS;
    	notSS << currTime;
		std::string currTimestamp = notSS.str();
		// for (size_t i = 0; i < channel->getOpsSize(); i++)
		// {
			sendToClient(buildReply(SERVER, name, 332, "", 2, channel->getName().c_str(), channel->getTopic().c_str()), client);
			sendToClient(buildReply(SERVER, name, 333, "", 3, channel->getName().c_str(), name.c_str(), currTimestamp.c_str()), client);
			sendToChannel(buildReply(SERVER, name, 332, "", 2, channel->getName().c_str(), channel->getTopic().c_str()), *channel, client);
			sendToChannel(buildReply(SERVER, name, 333, "", 3, channel->getName().c_str(), name.c_str(), currTimestamp.c_str()), *channel, client);

		// }
		// for (size_t i = 0; i < channel->getClientsSize(); i++)
		// {
			//sendToChannel(buildReply(SERVER, channel->getClients()[i].getNickname(), 332, "", 2, channel->getName().c_str(), channel->getTopic().c_str()), *channel, channel->getClients()[i]);
			//sendToChannel(buildReply(SERVER, channel->getClients()[i].getNickname(), 333, "", 3, channel->getName().c_str(), name.c_str(), currTimestamp.c_str()), *channel, channel->getClients()[i]);
		//}
	}
	if (!isOP && channel->getRestrictTopic())
		return(sendToClient(buildReply(SERVER, client.getNickname(), 482, "", 1, _channel.c_str()), client));
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

	if (_target.empty() || _channel.empty())
		return (sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "PRIVMSG"), client));
	if (channel == NULL)
		return (sendToClient(":ft_irc NOTICE " + name + ": No such channel" + END, client));
	if (!channel->clientIsOp(name))
		return(sendToClient(buildReply(SERVER, client.getNickname(), 482, "", 1, _channel.c_str()), client));
	if (!channel->clientIsInChannel(_target))
		return (sendToClient(":ft_irc 441 " + name + " " + _target + " " + _channel + ":User not on channel" + END, client));
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
		return (sendToClient(":ft_irc NOTICE " + name + " " + _channel + " :No such channel" + END, client));
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

	if (_target.empty() || _channel.empty())
		return (sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "PRIVMSG"), client));
	if (channel == NULL)
		return(sendToClient(buildReply(SERVER, client.getNickname(), 442, "", 1, _channel.c_str()), client));
	if (!findClient(_target))
		return (sendToClient(buildReply(SERVER, name, 401, "", 1, _target.c_str()), client));
	if (!channel->clientIsInChannel(name))
		return (sendToClient(":ft_irc 441 " + name + " " + _channel + " :You`re not on that Channel" + END, client));
	if (channel->clientIsInChannel(_target))
		return (sendToClient(":ft_irc 443 " + name + " " + _target + " " + _channel + " :is already on that channel" + END, client));
	if (!channel->clientIsOp(name) && channel->getInviteOnly())
		return(sendToClient(buildReply(SERVER, client.getNickname(), 482, "", 1, _channel.c_str()), client));
	channel->addInvitedClient(_target);
	sendToClient(":ft_irc 341 " + name + " " + _target + " " +_channel + END, client); // confirmation message to sender
	sendToClient(":" + name + " INVITE "+ _target + " " + _channel + END, *findClient(_target)); // invite to target user
	return (1);
}
