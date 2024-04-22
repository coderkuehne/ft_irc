#include "Server.hpp"

bool	isValidPort(const std::string&);

void printStringArray(char** strings) {
    for (int i = 0; strings[i]; ++i) {
        std::cout << strings[i] << std::endl;
    }
}


int main (int ac, char** av, char **env)
{
	printStringArray(env);

	if (ac != 3) {
		std::cerr << RED << "Usage: ./ircserv [port] [password]" << RESET << std::endl;
		return (1);
	}
	if (!isValidPort(av[1])) {
		std::cerr << RED << "Error: invalid port" << RESET << std::endl;
		return (1);
	}

	Server	server(av[1], av[2]);
	try {
		server.start();
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return (0);
}

bool	isValidPort(const std::string& port) {
	if (port.length() > 5)
		return false;
	for (std::string::const_iterator it = port.begin(); it != port.end(); ++it) {
		if (!isdigit(*it))
			return false;
	}
	if (std::atoi(port.c_str()) > 65535)
		return false;
	return true;
}
