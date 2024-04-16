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
	registerClient(client);
	return (0);
}

int	Server::setUsername(const std::string& user, Client &client) const
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
//	if (user[0] == '#' || user[0] == ':' || user[0] == ' ')
//	{
//		std::cerr << RED << "Invalid username" << RESET << std::endl;
//		sendToClient(":ft_irc 432 Erroneous username" + END, client);
//		return (-1);
//	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (user == _clients[i].getUsername())
		{
			std::cerr << RED << "Username already in use" << RESET << std::endl;
			sendToClient(":ft_irc 462 :User is already registered" + END, client);
			return (-1);
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
	for (size_t i = 0; i < _clients.size(); i++)
		sendToClient(":ft_irc 353 " + client.getNickname() + " = " + channel.getName() + " :" + _clients[i].getNickname() + END, client);
	sendToClient(":ft_irc 366 " + client.getNickname() + " " + channel.getName() + " :End of /NAMES list" + END, client);
}

int Server::joinChannel(std::string& channelName, std::string& key, Client &client)
{
	std::cout << "what is input " << channelName << " ." << std::endl;
	if (channelName.empty())
	{
		std::cout << "what is input " << client.getNickname() << " ." << std::endl;

		sendToClient(":ft_irc 461" + client.getNickname() + " JOIN " + ":Not enough parameters" + END, client);
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
			sendToChannel(":" + client.getNickname() + " JOIN " + _channels[i].getName() + END, _channels[i], client);
			return (0);
		}
	}
	Channel newChannel(channelName, key); //if no key is supplied, key is set to ""
	newChannel.addClient(client);

	addChannel(newChannel);

	std::cout << "hiello?" << std::endl;
	responseForClientJoiningChannel(client, newChannel);

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

int Server::SetTopic(Client &client, std::string& channel, std::string& newTopic)
{
	if (newTopic.empty())
	{
		sendToClient(":ft_irc 461" + client.getNickname() + " TOPIC " + ":Not enough parameters" + END, client);
		return (1);
	}
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (channel == _channels[i].getName())
		{
			if (client not connected to the channel)
			{
				sendToClient(":ft_irc 442" + client.getNickname() + " " + _channels[i].getName() + " :You're not on that channel" + END, client);
				return (1);
			}
			if (newTopic.empty())
			{
				//just print the current topic
			}
			else
			{
				//set the topic
				//let others know who set the topic and what is the topic 
			}
		}
		else
		{
			sendToClient(":ft_irc 403" + client.getNickname() + " " + channel + " :No such channel" + END, client);
			return (1);
		}
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
