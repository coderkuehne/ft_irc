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
	//std::cout << "what is input " << client.getNickname() << " and " << args[1][0] << " ." << std::endl;
	if (channelName.empty())
	{
//		sendToClient(":ft_irc 461 *" + client.getNickname() + " " + "JOIN" + " :Not enough parameters" + END, client);
		sendToClient(buildReply(SERVER, client.getNickname(), 461, "", 1, "JOIN"), client);
		return (1);
	}
	if (channelName[0] != '#')
	{
		sendToClient(":ft_irc 476 " + channelName + " :Bad Channel Mask" + END, client);
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

	// sendToClient(":" + client.getNickname() + " JOIN " + newChannel.getName() + END, client);
	// sendToClient(":ft_irc 332 " + client.getNickname() + " " + newChannel.getName() + " :" + newChannel.getTopic() + END, client);
	// for (size_t i = 0; i < _clients.size(); i++)
	// 	sendToClient(":ft_irc 353 " + client.getNickname() + " = " + newChannel.getName() + " :" + _clients[i].getNickname() + END, client);
	// sendToClient(":ft_irc 366 " + client.getNickname() + " " + newChannel.getName() + " :End of /NAMES list" + END, client);

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

void Server::printClients(void)
{
	std::cout << "Total Clients:" << _clients.size() << std::endl;
	for (size_t i = 0; i < _clients.size(); i++)
	{
		std::cout << "\tClient " << i << ": " << _clients[i].getNickname() << " Username : " << _clients[i].getUsername() << "on Socket :" << _clients[i].getSocket() << std::endl;
	}
}
