#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define PORT 12345
#define BUFFER_SIZE 64

sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
// Function prototypes
void client_wants_to_read( int new_socket, char *send_to_user);
void client_wants_to_write(char *argument);
void read_last_n_lines(FILE *file, int n, char *send_to_user);

int main() 
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char command[BUFFER_SIZE] = {0};
    char argument[BUFFER_SIZE] = {0};
    char send_to_user[BUFFER_SIZE*8];
    FILE *log_file;
    
    printf("START SERVER\n");
    //system("ifconfig eth0 10.0.2.17");
    
    
    
    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Bind success\n");

    // Listen for connections
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    // Accept incoming connections
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("Connection accepted\n");

    while(1)
    {
        // Read message from client and write to log file
        valread = read(new_socket, buffer, sizeof(buffer));
        if(valread <= 0) 
        {
            printf("Client disconnected\n");
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) 
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("Connection accepted\n");

        }
        printf("sever got :::::::: %s \n",buffer );
        // scan command and argument (read/write) 
        buffer[strlen(buffer)] = '\0';
        int ret = sscanf(buffer, "%s %s", command, argument);
        
        // if command is read
        if (strcmp(command, "read") == 0)
        {
            client_wants_to_read(new_socket, send_to_user);
        }
        // if command is write
        else if (strcmp(command, "write") == 0)
        {
            printf("Client wants to write - %s\n", buffer);
            buffer[strlen(buffer)] = '\0';
            client_wants_to_write(buffer+6);
            memset(buffer, 0, 64);

        }
        else
        {
            printf("Invalid command - %s \n",command);
        }
       
    }

    // Close socket
    close(new_socket);
    return 0;
}

//READ
void client_wants_to_read(int new_socket, char *send_to_user)
{
   
    // Open log file for reading
    FILE * log_file = fopen("server_log.txt", "r");
    if (log_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    printf("Log file opened\n");

    // Read from log file and send to client
    char *temp = send_to_user;
    read_last_n_lines(log_file, 8, temp);
    printf("sending to user..\n");
    send(new_socket, send_to_user, strlen(send_to_user), 0);
    
    
    fclose(log_file);

}

//WRITE
void client_wants_to_write(char *argument)
{
    printf("Client wants to write0 - %s\n", argument);
    
    FILE *log_file = fopen("server_log.txt", "a");
    if (log_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    printf("Log file opened\n");

    // Write to log file
    fprintf(log_file, "%s", argument);
    fclose(log_file);

}

void read_last_n_lines(FILE *file, int n, char *send_to_user) {
   
    long pos;
    int ch;
    int newline_count = 0;
    // Seek to the end of the file
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("fseek failed");
        return;
    }

    pos = ftell(file); // Current position
    if (pos == 0) { // Check if the file is empty
        printf("File is empty\n");
        return;
    }

    // Find the positions of the last n newline characters
    while (pos > 0 && newline_count < n) {
        fseek(file, --pos, SEEK_SET);
        if ((ch = fgetc(file)) == '\n') {
            newline_count++;
        }
    }

    // Move to the position after the last found newline (or start of the file)
    if (pos > 0) {
        fseek(file, pos + 1, SEEK_SET);
    } else {
        fseek(file, 0, SEEK_SET);
    }

    // Read and print the last n lines
    char line[256];
    char *temp = send_to_user;

    while (fgets(line, sizeof(line), file) != NULL) 
    {
        strcpy(temp, line);
        //temp[strlen(line)] = '\n';
        temp += strlen(line); // +1
    }

}
