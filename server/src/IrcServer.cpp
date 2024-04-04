/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kekuhne <kekuhne@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 13:28:32 by kekuhne           #+#    #+#             */
/*   Updated: 2024/04/04 17:48:40 by kekuhne          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../include/IrcServer.hpp"

Server::Server()
{
	//std::cout << "Default Server Consturctor called" << std::endl;
	_port = 6969;
	_ip = "127.0.0.1";
	_password = "Red Flag";
};

Server::Server(int port, std::string ip, std::string password)
{
	//std::cout << "Server Constructor called" << std::endl;
	_port = port;
	_ip = ip;
	_password = password;
};

Server::~Server()
{
	//std::cout << "Server Destructor called" << std::endl;
};

void Server::createSocket()
{
		_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (_socket == -1)
		{
			std::cerr << "Error creating socket" << std::endl;
			exit(1);
		}
};

void Server::bindSocket()
{
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(_port);
	_addr.sin_addr.s_addr = inet_addr(_ip.c_str());
	if (bind(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
	{
		std::cerr << "Error binding socket" << std::endl;
		exit(1);
	}
};

void Server::listenSocket()
{
	if (listen(_socket, 3) < 0)
	{
		std::cerr << "Error listening socket" << std::endl;
		exit(1);
	}
};

void Server::acceptSocket(std::string password)
{
	std::cout << "Waiting for connection..." << std::endl;
	if (password != _password)
	{
		std::cerr << "Error password does not match" << std::endl;
		exit(1);
	}
	int addrlen = sizeof(_addr);
	_socket = accept(_socket, (struct sockaddr *)&_addr, (socklen_t*)&addrlen);
	if (_socket < 0)
	{
		std::cerr << "Error accepting socket" << std::endl;
		exit(1);
	}
};

void Server::sendSocket(std::string message)
{
	send(_socket, message.c_str(), message.length(), 0);
};

int Server::receiveSocket(char *buffer, int BUFFER_SIZE)
{
	int bytes_received = recv(_socket, buffer, BUFFER_SIZE, 0);
	return (bytes_received);
};

void Server::closeSocket()
{
	close(_socket);
};
