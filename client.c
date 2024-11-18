#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];

    // Create client socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error: while creating client socket");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    // Check ip address of the server
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        perror("Error: invalide ip addres");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for connection
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error: while listening for connection");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    printf("We are connected to the server.\n");

    while (1) {
        printf("Enter a message (or 'exit' to left) : ");
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0';

        // Left if the user enter "exit"
        if (strcmp(message, "exit") == 0) {
            printf("logout...\n");
            break;
        }

        // Send the msg to server
        send(clientSocket, message, strlen(message), 0);

        // Listening the msg from server
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        buffer[bytesReceived] = '\0';
        printf("Server response: %s\n", buffer);
    }

    close(clientSocket);
    return 0;
}
