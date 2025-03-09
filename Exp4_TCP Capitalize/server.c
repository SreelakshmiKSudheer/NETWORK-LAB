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
    // server socket
    int server_socket;
    int port = atoi(argv[1]);

    // address structures
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // socket creation
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }

    // structure set up
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // binding
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Binding failed...");
        exit(1);
    }

    // listening
    if (listen(server_socket, 5) == -1)
    {
        perror("Listening failed...");
        exit(1);
    }
    printf("Listening for clients...\n");

    // accept
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) == -1)
    {
        perror("Client acceptance failed...");
        exit(1);
    }
    printf("Client accepted\n");

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0)
        {
            printf("Client disconnected.\n");
            break;
        }

        printf("Client: %s", buffer);

        // Exit condition
        if (strcasecmp(buffer, "exit\n") == 0)
        {
            printf("Client exiting...\n");
            break;
        }

        // Capitalization logic
        for (int i = 0; buffer[i] != '\0'; i++)
        {
            if (i == 0 || buffer[i - 1] == ' ' || buffer[i - 1] == '\t' || buffer[i - 1] == '\n')
            {
                if (buffer[i] >= 'a' && buffer[i] <= 'z')
                    buffer[i] -= 32;
            }
        }

        send(client_socket, buffer, strlen(buffer), 0);
        printf("Server: %s", buffer);
    }

    close(client_socket);
    close(server_socket);
}