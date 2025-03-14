#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int client_socket;
char buffer[BUFFER_SIZE];

int main(int argc, char *argv[])
{
    int port = argv[1];
    int server_socket; 
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }
    printf("Socket creation successful\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Socket binding failed");
        exit(1);
    }
    printf("Socket binding successful\n");

    while(1)
    {
        recvfrom(server_socket,)
    }

}