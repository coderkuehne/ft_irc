#include "Server.hpp"
#include "Parser.hpp"
#include "Client.hpp"

int	Server::authenticatePassword(Client& client, std::string& inputPassword) {
	if (inputPassword.empty()) {
			sendToClient(":ft_irc 461 * :No password provided" + END, client);
			return -1;
		}
	if (inputPassword == _password) {
		client.beAuthenticated();
		return 0;
	}
	else {
		sendToClient(":ft_irc 464 * :Password is incorrect" + END, client);
		return -1;
	}
}

int	Server::changeNickname(const std::string& nick, Client &client)
{
	if (nick.empty())
	{
		std::cerr << RED << "No nickname given" << RESET << std::endl;
		sendToClient(":ft_irc 431 :No nickname given" + END, client);
		return (-1);
	}
	if (nick[0] == '#' || nick[0] == ':' || nick[0] == ' ')
	{
		std::cerr << RED << "Invalid nickname" << RESET << std::endl;
		sendToClient(":ft_irc 432 :Erroneous nickname" + END, client);
		return (-1);
	}
	if (getClient(nick)) {
		std::cerr << RED << "Nickname already in use" << RESET << std::endl;
		sendToClient(":ft_irc 433 :Nickname is already in use" + END, client);
		return (-1);
	}
	client.setNickname(nick);
	sendToClient(":ft_irc NICK " + nick + END, client);
	registerClient(client);
	return (0);
}

int	Server::setUsername(std::string& user, Client &client)
{
	if (client.isRegistered()) {
		std::cerr << RED << "User already registered" << RESET << std::endl;
		sendToClient(":ft_irc 462 :User is already registered" + END, client);
		return (-1);
	}
	if (user.empty())
	{
		std::cerr << RED << "No username given" << RESET << std::endl;
		sendToClient(":ft_irc 431 :No username given" + END, client);
		return (-1);
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (user == _clients[i].getUsername())
		{
			std::cerr << RED << "Username already in use" << RESET << std::endl;
			sendToClient(":ft_irc NOTICE :Username is taken, registering as guest" + END, client);
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
		sendToClient(":ft_irc 001 " + client.getNickname() + " :Welcome to our ft_irc server, " + client.getNickname() + END, client);
	}
}

int Server::ChannelMessage(std::string& target, std::string& message, std::string& receivedString, Client &client)
{
	if (target.empty() || message.empty())
	{
		std::cerr << RED << "Invalid command" << RESET << std::endl;
		sendToClient(":ft_irc 461 * :Not enough parameters" + END, client);
		return (1);
	}

	Channel	*channel = getChannel(target);
	if (!channel)
	{
		std::cerr << RED << "Invalid target" << RESET << std::endl;
		sendToClient(":ft_irc 401 * :No such channel" + END, client);
		return (1);
	}
	sendToChannel(":" + client.getNickname() + " " + receivedString + END, *channel, client);
	return (0);
}

int Server::message(std::string& target, std::string& message, std::string& receivedString, Client &client)
{
	if (target.empty() || message.empty())
	{
		std::cerr << RED << "Invalid command" << RESET << std::endl;
		sendToClient(":ft_irc 461 * :Not enough parameters" + END, client);
		return 1;
	}

	if (target[0] == '#') {
		ChannelMessage(target, message, receivedString, client);
		return 1;
	}

	Client	*recipient = getClient(target);
	if (!recipient)
	{
		std::cerr << RED << "Invalid target" << RESET << std::endl;
		sendToClient(":ft_irc 401 * :No such user" + END, client);
		return 1;
	}
	sendToClient(":" + client.getNickname() + " " + receivedString + END, *recipient);
	return 0;
}

void Server::responseForClientJoiningChannel(Client &client, Channel &channel)
{
	sendToClient(":" + client.getNickname() + " JOIN " + channel.getName() + END, client);
	sendToClient(":ft_irc 332 " + client.getNickname() + " " + channel.getName() + " :" + channel.getTopic() + END, client);
}

void	Server::names(Client& client, std::string& channelName)
{
	Channel	*channel = getChannel(channelName);
	if (!channel)
	{
		sendToClient(":ft_irc 401 * :No such channel" + END, client);
		return;
	}
	std::string	list = channel->getClientList();
	sendToClient(":ft_irc 353 " + client.getNickname() + " = " + channelName + " :" + channel->getClientList() + END, client);
	sendToClient(":ft_irc 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list" + END, client);
//	sendToClient(":ft_irc 352 " + client.getNickname() + " " + channel->getName() + " " + list + END, client);
//	sendToClient(":ft_irc 315 " + client.getNickname() + " " + channel->getName() + " :End of /WHO list" + END, client);
}

int Server::joinChannel(std::string& channelName, std::string& key, Client &client)
{
	//std::cout << "what is input " << client.getNickname() << " and " << args[1][0] << " ." << std::endl;
	if (channelName.empty())
	{
		sendToClient(":ft_irc 461 *" + client.getNickname() + " " + "JOIN" + " :Not enough parameters" + END, client);
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
			sendToChannel(":" + client.getNickname() + " JOIN " + _channels[i].getName() + END, _channels[i], client);
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

int Server::quit(Client &client, std::string& quitMessage)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i] == client)
		{
			//part from all channels
			close(client.getSocket());
			_clients.erase(_clients.begin() + i);
			_fds.erase(_fds.begin() + i + 1);
		}
	}
	for (size_t i = 0; i < _clients.size(); ++i) {
		sendToClient(":" + client.getNickname() + " QUIT :Quit " + quitMessage + END, _clients[i]);
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
