/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kekuhne <kekuhne@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 12:48:26 by kekuhne           #+#    #+#             */
/*   Updated: 2024/04/04 18:28:37 by kekuhne          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IrcServer.hpp"
#define PORT 6969
#define BUFFER_SIZE 1024
#define IP "127.0.0.1"

int main (int argc, char **argv)
{
	char buffer[1024];

	buffer[0] = '\0';
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircServ <port> <password>" << std::endl;
		exit (1);
	}
	Server ircServ(atoi(argv[1]), IP, argv[2]);
	ircServ.createSocket();
	std::cout << "Socket created" << std::endl;
	ircServ.bindSocket();
	std::cout << "Socket binded" << std::endl;
	ircServ.listenSocket();
	std::cout << "Socket listening" << std::endl;
	ircServ.acceptSocket(argv[2]);
	std::cout << "Socket accepted" << std::endl;
	//how do get the Client socket? me no gusta
		ircServ.receiveSocket(buffer, BUFFER_SIZE);
		std::cout << "Received: " << static_cast<const char*>(buffer) << std::endl;
		ircServ.sendSocket("Message received");
	ircServ.closeSocket();
	return (0);
}