#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handleClient(int clientSocket, struct sockaddr_in clientAddr) {
    char buffer[BUFFER_SIZE];
    int bytesRead;

    int clientPort = ntohs(clientAddr.sin_port);

    while ((bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytesRead] = '\0';
        printf("Received message from client (port %d): %s\n", clientPort, buffer);

        write(clientSocket, buffer, bytesRead);
    }

    close(clientSocket);
    printf("Client (port %d) disconnected.\n", clientPort);
    exit(0);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // Create socket serveur
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error: while creating server socket");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // bind server socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error: while binding the socket with the addr");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(serverSocket, 5) < 0) {
        perror("Error: while listening for connections");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen)) < 0) {
            perror("Error: acceptation failed");
            continue;
        }

        printf("Client (port %d) connected\n",ntohs(clientAddr.sin_port));

        // Create child process for client
        if (fork() == 0) {
            close(serverSocket);
            handleClient(clientSocket, clientAddr);
        }

        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}
