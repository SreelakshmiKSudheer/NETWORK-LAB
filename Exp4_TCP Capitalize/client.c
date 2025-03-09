// header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// definitions
#define BUFFER_SIZE 1024

// global declarations
int client_socket;
char buffer[BUFFER_SIZE];

// main
int main(int argc, char *argv[])
{
    int port = atoi(argv[1]);

    // address structures
    struct sockaddr_in server_addr;

    // socket creation
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }

    // structure set up
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connection Failed...");
        exit(1);
    }
    printf("Connection Successful\n");

    while (1)
    {
        // Send message
        fgets(buffer, BUFFER_SIZE, stdin);
        send(client_socket, buffer, strlen(buffer), 0);

        // Exit condition
        if (strncmp(buffer, "exit", 4) == 0)
        {
            printf("Client exiting\n");
            break;
        }

        // Receive message
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0)
        {
            printf("Server disconnected.\n");
            break;
        }
        printf("Server: %s", buffer);
    }

    close(client_socket);
}