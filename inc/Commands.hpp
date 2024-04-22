#ifndef COMMANDS_HPP
# define COMMANDS_HPP

# include "IRC.hpp"

# define PASS 100
# define NICK 101
# define USER 102
# define PRIVMSG 103
# define JOIN 104
# define WHO 105
# define NOTICE 110
# define KICK 106
# define PART 107
# define INVITE 109
# define TOPIC 77 //should it be random?
# define QUIT 169
# define MODE 108

std::string	buildReply(const std::string& sender, const std::string& recipient, int messageCode, const std::string& message, int paramCount ...);

#endif
