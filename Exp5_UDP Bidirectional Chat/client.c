// header
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// definitions
#define BUFFER_SIZE 1024

char buffer[BUFFER_SIZE];

// main
int main(int argc, char* argv[])
{
        int port = atoi(argv[1]);
        // socket
        int client_socket;
        // structures
        struct sockaddr_in server_addr, client_addr;
        socklen_t server_len = sizeof(server_addr);

        // socket creation
        client_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if(client_socket == -1)
        {
                perror("Socket Creation Failed");
                exit(1);
        }
        printf("Socket Creation Successful\n");

        // address set up
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        printf("Socket Established\n");

        while(1)
        {
                // read
                fgets(buffer, BUFFER_SIZE, stdin);
                // send
                sendto(client_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, server_len);
                if(strcmp(buffer, "exit\n") == 0)
                {
                        printf("Client exiting\n...");
                        break;
                }
                // receive
                memset(buffer, 0, BUFFER_SIZE);
                recvfrom(client_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &server_len);
                printf("Server: %s", buffer);

                if(strcmp(buffer, "exit\n") == 0)
                {
                        printf("Server exiting\n...");
                        break;
                }
        }
}