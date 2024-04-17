#ifndef COMMANDS_HPP
# define COMMANDS_HPP

# include "Server.hpp"

# define PASS 100
# define NICK 101
# define USER 102
# define PRIVMSG 103
# define JOIN 104
# define WHO 105
# define QUIT 169

std::string	buildReply(const std::string& sender, Client& recipient, int messageCode, const std::string& message, int paramCount ...);

#endif
