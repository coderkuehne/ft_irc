/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kekuhne <kekuhne@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 13:13:15 by kekuhne           #+#    #+#             */
/*   Updated: 2024/04/04 17:46:28 by kekuhne          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_SERVER_HPP
# define IRC_SERVER_HPP
# include <iostream>
# include <string>
# include <cstring>
# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <exception>
# include <map>

	class Client;
	class Server
	{
		private:
			int _port;
			std::string _ip;
			int _socket;
			struct sockaddr_in _addr;
			/* char _buffer[1024]; */
			std::string _password;
			/* std::map<int, Client> _clients; */
		public:
			Server();
			Server(int port, std::string ip, std::string password);
			~Server();
			void createSocket();
			void bindSocket();
			void listenSocket();
			void acceptSocket(std::string password);
			void sendSocket(std::string message);
			int receiveSocket(char *buffer, int BUFFER_SIZE);
			void closeSocket();
	};
#endif