#include "Server.hpp"
#include "Parser.hpp"
#include "Client.hpp"

bool	Server::authenticatePassword(Client& client, std::string& inputPassword) {
	std::string	password;
	switch(getPassword(inputPassword, password)) {
		case NO_PASS: {
			sendToClient("ERROR :This server requires a password" + END, client);
			return false;
		}
		case NO_DELIM:
			return false;
		case NO_PARAM: {
			sendToClient(":ft_irc 461 * :No password provided" + END, client);
			break;
		}
		default: {
			if (password == _password)
				return true;
			sendToClient(":ft_irc 464 * :Password is incorrect" + END, client);
			return false;
		}
	}
	return false;
}

bool	Server::registerClientNames(Client& client, std::string& registrationData) {
	std::string	nick = "";
	std::string	user = "";

	getNames(registrationData, nick, user);
	if (checkClientRegistered(user)) {
		sendToClient(":ft_irc 462 * :Username already registered" + END, client);
	}
	else if (!user.empty())
		client.setUsername(user);
	else
		client.setUsername("Guesteroni");

	client.setNickname(nick);

	if (!client.getNickname().empty() && !client.getUsername().empty())
		return true;
	return false;
}
