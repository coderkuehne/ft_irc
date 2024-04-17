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
# include <ctime>

# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define RESET "\033[0m"

# define BUFFER_SIZE 1024
# define DEBUG 1

# define ERR_NONICKNAMEGIVEN 431
# define ERR_ERRONEUSNICKNAME 432
# define ERR_NICKNAMEINUSE 433
# define ERR_NEEDMOREPARAMS 461

const std::string	END = "\r\n";

class Client;
typedef std::vector<std::string>::iterator	stringIt;
typedef std::vector<Client>::iterator		clientIt;

#endif
