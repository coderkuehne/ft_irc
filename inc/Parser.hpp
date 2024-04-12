#ifndef PARSER_HPP
# define PARSER_HPP

# define NO_PASS 20
# define NO_DELIM 21
# define NO_PARAM 22

std::vector<std::string>	splitStringByEND(const std::string&);
int							convertCommand(const std::string&);

int		getPassword(std::string, std::string&);
void	getNames(std::string, std::string&, std::string&);

#endif
