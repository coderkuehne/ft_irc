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

#include "../include/IrcServer.hpp"


/* 
int main (int argc, char **argv)
{
	(void)argc;
	(void)argv;
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircServ <port> <password>" << std::endl;
		return (1);
	}
	Server ircServ(atoi(argv[1]), IP, argv[2]);
	Server ircServ(PORT, IP, "RED FLAG");
	ircServ.createSocket();
	ircServ.bindSocket();
	ircServ.listenSocket();
	while (true)
	{
		int client_socket = ircServ.acceptSocket(argv[2]);
	}
	ircServ.receiveSocket(client_socket);
	
	ircServ.sendSocket("Message received", client_socket);
	ircServ.closeSocket(client_socket);
	return (0);
}
 */

int main ()
{
	Server ircServ(PORT, IP, "RED FLAG");

	std::cout << "\t****Server****" << std::endl;
	try
	{
		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		ircServ.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		ircServ.closeSocket();
	}
	std::cout << "Server shutdown" << std::endl;
	return (0);
}