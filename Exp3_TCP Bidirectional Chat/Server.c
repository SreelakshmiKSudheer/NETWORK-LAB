// header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// definitions
#define BUFFER_SIZE 1024

// global declarations
int client_socket;

// receive
void *receiveMsg(void *arg)
{
        char buffer[BUFFER_SIZE];

        while(1)
        {
                memset(buffer, 0, BUFFER_SIZE);
                recv(client_socket, buffer, BUFFER_SIZE, 0);
                printf("Client: %s", buffer);
                if(strcasecmp(buffer, "exit\n") == 0)
                {
                        printf("Client exiting...\n");
                        break;
                }
        }
        return NULL;
}

// send
void *sendMsg(void *arg)
{
        char buffer[BUFFER_SIZE];

        while(1)
        {
                fgets(buffer, BUFFER_SIZE, stdin);
                send(client_socket, buffer, BUFFER_SIZE, 0);
                if(strcasecmp(buffer, "exit\n") == 0)
                {
                        printf("Server exiting...\n");
                        break;
                }
        }
        return NULL;
}

// main
int main(int argc, char *argv[])
{
// server socket
        int server_socket;
        int port = atoi(argv[1]);
// address structures 2
        struct sockaddr_in server_addr, client_addr;
        socklen_t client_len = sizeof(client_addr);
// thread
        pthread_t receiveThread, sendThread;
// socket creation
        server_socket = socket(AF_INET, SOCK_STREAM, 0);

        if(server_socket == -1)
        {
                perror("Socket creation failed");
                exit(1);
        }
// structure set up
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

// binding
        if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
        {
                perror("Binding failed...");
                exit(1);
        }
// listening
        if(listen(server_socket,5) == -1)
        {
                perror("Listening failed...");
                exit(1);
        }
        printf("Listening for clients...\n");
// accept
        if((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len)) == -1)
        {
                perror("Client acception failed...");
                exit(1);
        }
        printf("Client accepted\n");
// thread creation
        pthread_create(&receiveThread, NULL, receiveMsg, NULL);
        pthread_create(&sendThread, NULL, sendMsg, NULL);
// thread join
        pthread_join(receiveThread, NULL);
        pthread_join(sendThread, NULL);

        close(client_socket);
        close(server_socket);
}