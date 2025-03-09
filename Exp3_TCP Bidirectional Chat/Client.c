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
                printf("Server: %s", buffer);
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
       int port = atoi(argv[1]);
// address structures 2
        struct sockaddr_in server_addr;
// thread
        pthread_t receiveThread, sendThread;
// socket creation
        client_socket = socket(AF_INET, SOCK_STREAM, 0);

        if(client_socket == -1)
        {
                perror("Socket creation failed");
                exit(1);
        }
// structure set up
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
                perror("Connection Failed...");
                exit(1);
        }
        printf("Connection Successful\n");
        // thread creation
        pthread_create(&receiveThread, NULL, receiveMsg, NULL);
        pthread_create(&sendThread, NULL, sendMsg, NULL);
// thread join
        pthread_join(receiveThread, NULL);
        pthread_join(sendThread, NULL);

        close(client_socket);
}