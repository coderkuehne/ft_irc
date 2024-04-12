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

int	Server::changeNickname(std::string nick, Client &client)
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

int	Server::setUsername(std::string user, Client &client)
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

void	Server::registerClient(Client &client) {
	if (!client.isRegistered() && !client.getNickname().empty() && !client.getUsername().empty()) {
		client.beRegistered();
		sendToClient(":ft_irc 001 " + client.getNickname() + " :Welcome to our ft_irc server, " + client.getNickname() + END, client);
	}
}

int Server::cmd_msg(std::vector<std::string> args, size_t msg_size, Client &client)
{
	Client	*target;

	if (msg_size < 3)
	{
		std::cerr << RED << "Invalid command" << RESET << std::endl;
		sendToClient(":ft_irc 461 * :Not enough parameters" + END, client);
		return (0);
	}
	if (args[1][0] == '#')
	{
		//channel message
		return (0);
	}
	else
	{		
		target = getClient(args[1]);
		if (!target)
		{
			std::cerr << RED << "Invalid target" << RESET << std::endl;
			sendToClient(":ft_irc 401 * :No such nick/channel" + END, client);
			return (0);
		}
	}
	std::string		message;
	for (size_t i = 1; i < msg_size; i++)
		message += args[i] + " ";
	sendToClient(":" + client.getNickname() + " PRIVMSG " + target->getNickname() + " :" + message + END, *target);
	return (1);
}

int Server::joinChannel(std::vector<std::string> args, Client &client)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (args[1] == _channels[i].getName())
		{
			_channels[i].addClient(client);
			sendToClient(":ft_irc 332 * :"+ _channels[i].getTopic() + END, client);
			return (0);
		}
	}
	Channel newChannel(args[1], "");
	if (args.size() == 3)
		newChannel.setKey(args[2]);
	
	newChannel.addClient(client);
	addChannel(newChannel);
	std::cout << "Are you here? " << client.getNickname() << " and " << _clients[0].getNickname() << " in " << newChannel.getName() << std::endl;
	sendToClient(":" + client.getNickname() + " JOIN " + newChannel.getName() + END, client);
	sendToClient(":ft_irc 332 " + client.getNickname() + " " + newChannel.getName() + " :" + newChannel.getTopic() + END, client);
	for (size_t i = 0; i < _clients.size(); i++)
		sendToClient(":ft_irc 353 " + client.getNickname() + " = " + newChannel.getName() + " :" + _clients[i].getNickname() + END, client);
    sendToClient(":ft_irc 366 " + client.getNickname() + " " + newChannel.getName() + " :End of /NAMES list" + END, client);
	
	
	std::cout << newChannel.getName() <<"Are you alive?" << std::endl;
	return (0);
}

int Server::cmd_quit(Client &client)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getSocket() == client.getSocket())
		{
			//part from all channels
			_clients.erase(_clients.begin() + i);
			_fds.erase(_fds.begin() + i + 1);
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
