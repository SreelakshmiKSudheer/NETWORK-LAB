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
        int server_socket;
        // structures
        struct sockaddr_in server_addr, client_addr;
        socklen_t client_len = sizeof(client_addr);

        // socket creation
        server_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if(server_socket == -1)
        {
                perror("Socket Creation Failed");
                exit(1);
        }
        printf("Socket Creation Successful\n");

        // address set up
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        // bind
        if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
                perror("Socket Binding Failed");
                exit(1);
        }
        printf("Socket Binding Successful\n");

        while(1)
        {
                // receive
                memset(buffer, 0, BUFFER_SIZE);
                recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_len);
                printf("Client: %s", buffer);

                if(strcmp(buffer, "exit\n") == 0)
                {
                        printf("Client exiting\n...");
                }
                // read
                fgets(buffer, BUFFER_SIZE, stdin);
                // send
                sendto(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, client_len);
                if(strcmp(buffer, "exit\n") == 0)
                {
                        printf("Server exiting\n...");
                        break;
                }

        }
}

/*
cc server.c -o server.out
./server.out 2305

SAMPLE OUTPUT

socket creation successful
socket binding successful
Client: Hi
Hi
Client: Nice to meet you
Introduce your self
Client: you don't know
quick, introduce
Client: i'm naruto
okay
Client: eixt
exit
Server exiting..
*/