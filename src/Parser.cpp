#include "Server.hpp"
#include "Parser.hpp"
#include "Commands.hpp"

void	Server::parseCommand(std::string command, Client& client) {
	std::vector <std::string> commands;

	for (std::vector<std::string>::iterator it = strings.begin(); it != strings.end(); ++it) {
		std::string command;
		std::istringstream iss(*it);
		iss >> command;

		switch(convertCommand(command)) {
			case QUIT: {
				//quit();
				break;
			}
			case PASS: {
				authenticatePassword(client, *it);
				break;
			}
			case NICK: {

				break;
			}
			case USER: {

				break;
			}
			case JOIN: {
				//join();
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
		tokens.push_back(str.substr(start, end - start + 2));
		start = end + END.length();
		end = str.find(END, start);
	}
	tokens.push_back(str.substr(start));
	return tokens;
}

int	getPassword(std::string registrationData, std::string& receivedPassword)
{
	size_t	passPos = registrationData.find("PASS ");
	size_t	delim = registrationData.find(END);

	if (passPos == std::string::npos)
		return NO_PASS;
	if (delim == std::string::npos)
		return NO_DELIM;
	if (delim - passPos == 1)
		return NO_PARAM;
	receivedPassword = registrationData.substr(passPos + 5, delim - passPos - 5);
	return 0;
}

void	getNames(std::string registrationData, std::string& nick, std::string& user)
{
	size_t nickPos = registrationData.find(END + "NICK ");
	size_t userPos = registrationData.find(END + "USER ");
	size_t delim = registrationData.find("0 *");

	if (nickPos != std::string::npos && userPos != std::string::npos) {
		nick = registrationData.substr(nickPos + 7, userPos - nickPos - 6);
	}
	if (userPos != std::string::npos && delim != std::string::npos) {
		user = registrationData.substr(userPos + 7, delim - userPos - 7);
	}
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
	return 0;
}
