#ifndef PARSER_HPP
# define PARSER_HPP

std::vector<std::string>	splitStringByEND(const std::string&);
int							convertCommand(const std::string&);

int		getPassword(std::string, std::string&);
void	getNames(std::string, std::string&, std::string&);

#endif
