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
                printf("Client: %s", buffer);
                if(strcasecmp(buffer, "exit\n") == 0)
                {
                        printf("Client exiting..\n");
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
                        printf("Server exiting..\n");
                        break;
                }
        }
}

// main
int main(int argc, char *argv[])
{
        // sockets, port
        int server_socket;
        int port = atoi(argv[1]);

        // address, len, thread
        struct sockaddr_in server_addr, client_addr;
        socklen_t client_len = sizeof(client_addr);

        pthread_t receiveThread, sendThread;

        // socket creation
        if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror("Socket Creation Failed");
                exit(1);
        }
        printf("Socket Creation Successful\n");

        // address setup
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        // binding
        if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
                perror("Socket Binding Failed");
                exit(1);
        }
        printf("Socket Binding Successful\n");

        // listening
        if(listen(server_socket,5) == -1)
        {
                perror("Listening Failed");
                exit(1);
        }
        printf("Listening for clients...\n");

        // accept
        if((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len)) == -1)
        {
                printf("Couldn't connect to client\n");
                exit(1);
        }
        printf("Connected to client\n");

        // thread creation
        pthread_create(&receiveThread,NULL,receiveMsg,NULL);
        pthread_create(&sendThread,NULL,sendMsg,NULL);

        // thread joining
        pthread_join(receiveThread, NULL);
        pthread_join(sendThread, NULL);

        // closing
        close(client_socket);
        close(server_socket);

        return 0;
}


/*
cc server.c -o server.out
./server.out 6245

USE ANY PORT NUMBER

Sample Output:


Socket Creation Successful
Socket Binding Successful
Listening for clients...
Connected to client
Hi
Client: Hey
Client: How are you
Fine fine
You talk
Client: Hey i am asta from Hage village
Hey asta
Client: exit
Client exiting..
exit
Server exiting..
*/