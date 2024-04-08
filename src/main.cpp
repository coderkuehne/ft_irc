/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kekuhne <kekuhne@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 12:48:26 by kekuhne           #+#    #+#             */
/*   Updated: 2024/04/07 12:32:54 by kekuhne          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool	isValidPort(const std::string&);

int main (int ac, char** av)
{
	if (ac != 3) {
		std::cerr << RED << "Usage: ./ircserv [port] [password]" << RESET << std::endl;
		return 1;
	}
	if (!isValidPort(av[1])) {
		std::cerr << RED << "Error: invalid port" << RESET << std::endl;
	}

	Server	server(av[1], av[2]);

	std::cout << "\t****Server****" << std::endl;
	try
	{
		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		server.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		server.closeSocket();
	}
	std::cout << "Server shutdown" << std::endl;
	return (0);
}

bool	isValidPort(const std::string& port) {
	if (port.length() > 5)
		return false;
	for (std::string::const_iterator it = port.begin(); it != port.end(); ++it) {
		if (!isdigit(*it))
			return false;
	}
	return true;
}
