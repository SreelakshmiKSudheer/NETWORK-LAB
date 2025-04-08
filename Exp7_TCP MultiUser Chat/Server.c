#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define MAX 10

int client_sockets[MAX], client_count = 0;
char buffer[BUFFER_SIZE];
pthread_mutex_t mutex;

void *server_client(void *arg);
void broadcastMsg(char msg[], int len);

int main(int argc, char *argv[])
{
        // port
        int port = atoi(argv[1]);

        // declarations
        int server_socket, client_socket;
        struct sockaddr_in client_addr, server_addr;
        socklen_t client_len;
        pthread_t tid;

        pthread_mutex_init(&mutex, NULL);

        // socket creation
        if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror("socket creation failed");
                exit(1);
        }
        printf("socket creation successful\n");

        // address set up
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        // binding
        if((bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) == -1)
        {
                perror("binding failed");
                exit(1);
        }
        printf("socket binding successful\n");

        if(listen(server_socket, 5) == -1)
        {
                perror("listening failed");
                exit(1);
        }
        printf("Server listening\n");

        printf("Starting the group chat on port: %d\n", port);

        while(1)
        {
                client_len = sizeof(client_addr);
                client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
                if(client_socket == -1)
                {
                        continue;
                }

                pthread_mutex_lock(&mutex);
                client_sockets[client_count++] = client_socket;
                pthread_mutex_unlock(&mutex);

                pthread_create(&tid, NULL, server_client, (void *)&client_socket);
                pthread_detach(tid);

        }
        close(server_socket);
        return 0;
}


void *server_client(void *arg)
{
        int client_socket = *(int *)arg, i;
        char buffer[BUFFER_SIZE];

        memset(buffer, 0, BUFFER_SIZE);

        while(recv(client_socket, buffer, BUFFER_SIZE, 0) > 0)
        {
                printf("%s", buffer);
                broadcastMsg(buffer, strlen(buffer));
        }

        pthread_mutex_lock(&mutex);

        for( i = 0; i < client_count; i++)
        {
                if(client_sockets[i] == client_socket)
                {
                        while(i < client_count)
                        {
                                client_sockets[i] = client_sockets[i+1];
                                i++;
                        }
                }
        }
        client_count--;
        pthread_mutex_unlock(&mutex);

        close(client_socket);
        return NULL;
}

void broadcastMsg(char msg[], int len)
{
        int i;

        pthread_mutex_lock(&mutex);
        for(i = 0; i < client_count; i++)
        {
                send(client_sockets[i], msg, len, 0);
        }
        pthread_mutex_unlock(&mutex);
}

/*
cc server.c -o server.out
./server.out 6001

SAMPLE OUTPUT:

socket creation successful
socket binding successful
Server listening
Starting the group chat on port: 6001
----- tony has joined the chat-----
----- steve has joined the chat-----
----- natasha has joined the chat-----
tony: so we are all here
natasha: yeah, cap what's your call
steve: we have to split
natasha: sure
tony: your plan?
steve: 3 stone, 3 avengers, 1 shot
tony: roger that
natasha: copy that
tony: exit
natasha: exit
steve: good luck guys
steve: bye bye
steve: exit
exit
^C
*/