/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kekuhne <kekuhne@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 12:48:26 by kekuhne           #+#    #+#             */
/*   Updated: 2024/04/04 13:57:07 by kekuhne          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./include/IrcServer.hpp"
#define PORT 6969
#define IP "127.0.0.1"

int main (int argc, char **argv)
{
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
	
	return (0);
}