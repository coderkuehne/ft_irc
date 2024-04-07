/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kekuhne <kekuhne@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 12:48:26 by kekuhne           #+#    #+#             */
/*   Updated: 2024/04/06 15:20:57 by kekuhne          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>


const int PORT = 6969;
const int BUFFER_SIZE = 1024;

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Resolve server hostname
    struct hostent *server = gethostbyname("localhost");
    if (server == NULL) {
        std::cerr << "Error resolving hostname\n";
        return 1;
    }

    // Set server address
    bzero((char*)&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&serverAddr.sin_addr.s_addr, server->h_length);
    serverAddr.sin_port = htons(PORT);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error connecting to server\n";
        return 1;
    }

    std::cout << "Connected to server\n";

    // Send data to server
   	char *msg;
	size_t msg_len;
	ssize_t bytes_sent;
    while (1) {
        printf("Enter message: ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            // Error or EOF occurred
            break;
        }
        // Remove newline character from input
        buffer[strcspn(buffer, "\n")] = '\0';
		if (strcmp(buffer, "exit") == 0)
			exit(0);
        // Send message to server
        msg = buffer;
        msg_len = strlen(msg);
        bytes_sent = send(clientSocket, msg, msg_len, 0);
        if (bytes_sent == -1) {
            fprintf(stderr, "send error: %s\n", strerror(errno));
            break;
        } else if ((size_t)bytes_sent == msg_len) {
            printf("Sent full message: \"%s\"\n", msg);
        } else {
            printf("Sent partial message: %zd bytes sent.\n", bytes_sent);
        }
    }

    // Receive data from server
    memset(buffer, 0, BUFFER_SIZE);
    int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
    if (bytesRead <= 0) {
        std::cerr << "Connection closed by server\n";
        close(clientSocket);
        return 1;
    }

    std::cout << "Received from server: " << buffer << std::endl;

    // Close socket
    close(clientSocket);

    return 0;
}