#ifndef IRC_HPP
# define IRC_HPP

# include <iostream>
# include <string>
# include <cstring>
# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <algorithm>
# include <exception>
# include <fcntl.h>
# include <vector>
# include <map>
# include <netinet/in.h>
# include <csignal>
# include <poll.h>
# include <sstream>
# include <cstdlib>
# include <sstream>
# include <iomanip>
# include <cstdarg>

# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define RESET "\033[0m"

# define BUFFER_SIZE 1024

# define ERR_NONICKNAMEGIVEN 431
# define ERR_ERRONEUSNICKNAME 432
# define ERR_NICKNAMEINUSE 433
# define ERR_NEEDMOREPARAMS 461

const	std::string	END = "\r\n";

inline std::map<int, std::string> generateNumericReplies() {
	std::map<int, std::string> map;
	map[001] = ":Welcome to our ft_irc server, ";
	map[366] = ":End of /NAMES list";
	map[401] = ":No such nick";
	map[403] = ":No such channel";
	map[404] = ":Cannot send to channel";
	map[431] = ":Not enough parameters";
	map[432] = ":Erroneous nickname";
	map[433] = ":Nickname already in use";
	map[461] = ":Not enough parameters";
	map[462] = ":User is already registered";
	map[464] = ":Password is incorrect";
	map[476] = ":Bad channel mask";
	return map;
}

const std::map<int, std::string> NUMERIC_REPLIES = generateNumericReplies();

class Client;
typedef std::vector<std::string>::iterator	stringIt;
typedef std::vector<Client>::iterator		clientIt;

#endif
