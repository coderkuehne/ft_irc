#include "Server.hpp"
#include "Parser.hpp"
#include "Commands.hpp"

void	Server::parseCommand(const std::string& clientPackage, Client& client) {
	std::vector <std::string> commands = splitStringByEND(clientPackage);

	for (std::vector<std::string>::iterator it = commands.begin(); it != commands.end(); ++it) {
		std::string			command;
		std::istringstream	not_ss(*it);
		not_ss >> command;

		std::string	parameter = "";
		not_ss >> parameter; //this should always be the first argument after command

		std::string	parameter2 = "";
		not_ss >> parameter2;

		int	cmd = convertCommand(command);
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
			case TOPIC: {
				cmdTopic(client, parameter, parameter2);
				break;
			}
			case PRIVMSG: {
				message(parameter, parameter2, *it, client);
				break;
			}
			case WHO: {
				names(client, parameter);
			}
			case KICK: {
				kickClient(parameter, parameter2, client);
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

int	convertCommand(const std::string& command) {
	if (command == "QUIT") {
		return QUIT;
	}
	else if (command == "PASS") {
		return PASS;
	}
	else if (command == "NICK") {
		return NICK;
	}
	else if (command == "USER") {
		return USER;
	}
	else if (command == "PRIVMSG") {
		return PRIVMSG;
	}
	else if (command == "JOIN") {
		return JOIN;
	}
	else if (command == "WHO") {
		return WHO;
	}
	else if (command == "TOPIC") 
	{
		return TOPIC;
	}
	else if (command == "KICK") {
		return KICK;
	}
	return 0;
}
