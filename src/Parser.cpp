#include "IRC.hpp"

void	Server::parseCommand(const std::string& clientPackage, Client& client) {
	std::vector <std::string> commands = splitStringByEND(clientPackage);

	for (stringIt it = commands.begin(); it != commands.end(); ++it) {
		std::string			command;
		std::istringstream	notSS(*it);
		notSS >> command;
		std::string	parameter;
		notSS >> parameter;
		std::string	parameter2;
		notSS >> parameter2;
		std::string	parameter3;
		notSS >> parameter3;

		std::string	message;
		size_t		colon = clientPackage.find(':', 1);
		if (colon != std::string::npos)
			message = clientPackage.substr(colon);
		else
			message = "";

		int	cmd = commandToMacro(command);
		switch (cmd) {
			case QUIT: {
				quit(client, parameter);
				return;
			}
			case PASS: {
				if (authenticatePassword(client, parameter) < 0)
					return;
				continue;
			}
			case 0:
				continue;
			default:
				break;
		}
		if (!client.isAuthenticated()) {
			sendToClient("ERROR :This server requires a password" + END, client);
			return;
		}

		switch(cmd) {
			case NICK: {
				changeNickname(parameter, client);
				break;
			}
			case USER: {
				setUsername(parameter, client);
				break;
			}
			case JOIN: {
				joinChannel(parameter, parameter2, client);
				break;
			}
			case PART: {
				partChannel(parameter, message, client);
				break;
			}
			case TOPIC: {
				channelTopic(parameter, message, client);
				break;
			}
			case PRIVMSG: {
				sendMessage(parameter, message, client);
				break;
			}
			case WHO: {
				names(client, parameter);
				break ;
			}
			case KICK: {
				kickClient(parameter, parameter2, message, client);
				break;
			}
			case INVITE:{
				inviteChannel(parameter, parameter2, client);
				break;
			}
			case MODE: {
				findChannel(parameter)->mode(parameter2, parameter3 ,client);
			break;
			}
			default: {
				break;
			}
		}
	}
}

std::vector<std::string>	splitStringByEND(const std::string& str) {
	std::vector<std::string>	tokens;
	std::string::size_type		start = 0;
	std::string::size_type		end = str.find(END);

	while (end != std::string::npos) {
		tokens.push_back(str.substr(start, end - start));
		start = end + END.length();
		end = str.find(END, start);
	}
	return tokens;
}

int	commandToMacro(const std::string& command) {
	if (command == "QUIT") {
		return (QUIT);
	}
	else if (command == "PASS") {
		return (PASS);
	}
	else if (command == "NICK") {
		return (NICK);
	}
	else if (command == "USER") {
		return (USER);
	}
	else if (command == "PRIVMSG") {
		return (PRIVMSG);
	}
	else if (command == "JOIN") {
		return (JOIN);
	}
	else if (command == "WHO") {
		return (WHO);
	}
	else if (command == "KICK") {
		return (KICK);
	}
	else if (command == "PART") {
		return (PART);
	}
	else if (command == "TOPIC") {
		return (TOPIC);
	}
	else if (command == "INVITE") {
		return (INVITE);
	}
	else if (command == "MODE") {
		return (MODE);
	}
	return (0);
}

std::string	macroToCommand(int command) {
	if (command == QUIT) {
		return ("QUIT");
	}
	else if (command == PASS) {
		return ("PASS");
	}
	else if (command == NICK) {
		return ("NICK");
	}
	else if (command == USER) {
		return ("USER");
	}
	else if (command == PRIVMSG) {
		return ("PRIVMSG");
	}
	else if (command == JOIN) {
		return ("JOIN");
	}
	else if (command == WHO) {
		return ("WHO");
	}
	else if (command == INVITE) {
		return ("INVITE");
	}
	else if (command == KICK) {
		return ("KICK");
	}
	else if (command == PART) {
		return ("PART");
	}
	else if (command == TOPIC) {
		return ("TOPIC");
	}
	else if (command == MODE) {
		return ("MODE");
	}
	else if (command == NOTICE) {
		return ("NOTICE");
	}
	else if (command > 0) {
		std::stringstream	ss;
		ss << std::setw(3) << std::setfill('0') << command;
		return ss.str();
	}
	return ("");
}
