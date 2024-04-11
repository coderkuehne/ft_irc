#include "Server.hpp"
#include "Parser.hpp"

int	getPassword(std::string registrationData, std::string& receivedPassword)
{
	size_t	passPos = registrationData.find("PASS ");
	size_t	delim = registrationData.find(END + ("NICK "));

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
