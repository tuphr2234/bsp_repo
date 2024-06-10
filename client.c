#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define SERVER_ADDR "10.0.2.17"

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    // default message
    char message[64] = "Hello, server! This is a log message.";
    char command[64] = {0};
    char argument[64] = {0}; 
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, SERVER_ADDR, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Server is not connected");
        exit(EXIT_FAILURE);
    }

    printf("print <write>, followed by log message, or <read> to read last 8 logs\n");

    while(1)
    {
        // scan message including spaces
        fgets(message, 64, stdin);

        //message[strlen(message)-1] = '\0'; 

        int ret = sscanf(message, "%s %s", command, argument);

        if(0 == strcmp(command, "write"))
        {
            // Send message to server
            message[strlen(message)] = '\0';
            message[strlen(message)+1] = '\0';
            send(sock, message, strlen(message), 0);
            // clean message
            memset(message, 0, 64);
        }
        else if(0 == strcmp(command, "read"))
        {
            // Send message to server
            send(sock, message, strlen(message), 0);

            // Read message from server
            char buffer[64*8] = {0};
            read(sock, buffer, 64*8);
            
            // prints logs to client
            printf("%s\n", buffer);
        }
        else
        {
            printf("Invalid command\n");
            printf("type <write>, followed by log message, or <read> to read last 8 logs\n");
        }
    }

    // Close socket
    close(sock);
    return 0;
}