// 7 header
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// macros
#define BUFFER_SIZE 1024

// global
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
                        printf("Server exiting..\n");
                        break;
                }
        }
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
                        printf("Client exiting..\n");
                        break;
                }
        }
}

// main
int main(int argc, char *argv[])
{
        int port = atoi(argv[1]);

        // address, len, thread
        struct sockaddr_in server_addr, client_addr;
        socklen_t server_len = sizeof(server_addr);

        pthread_t receiveThread, sendThread;

        // socket creation
        if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror("Socket Creation Failed");
                exit(1);
        }
        printf("Socket Creation Successful\n");

        // address setup
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET,"127.0.0.1",&server_addr.sin_addr);

        // binding
        if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
                perror("Socket Connection Failed");
                exit(1);
        }
        printf("Socket Connection Successful\n");

        // thread creation
        pthread_create(&receiveThread,NULL,receiveMsg,NULL);
        pthread_create(&sendThread,NULL,sendMsg,NULL);

        // thread joining
        pthread_join(receiveThread, NULL);
        pthread_join(sendThread, NULL);

        close(client_socket);
        return 0;
}

/*
cc client.c -o client.out
./client.out 6245

USE ANY PORT NUMBER

Sample Output: 

Socket Creation Successful
Socket Connection Successful
Server: Hi
Hey
How are you
Server: Fine fine
Server: You talk
Hey i am asta from Hage village
Server: Hey asta
exit
Client exiting..
Server: exit
Server exiting..
*/

