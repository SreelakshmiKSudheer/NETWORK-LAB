#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define MSG_SIZE 1000

int client_socket;
char name[24];
char msg[MSG_SIZE], buffer[BUFFER_SIZE];

void *receiveMsg(void *arg);
void *sendMsg(void *arg);

int main(int argc, char *argv[])
{
        // port
        int port = atoi(argv[1]);
        sprintf(name, "%s", argv[2]);

        // declarations
        struct sockaddr_in client_addr, server_addr;
        socklen_t server_len = sizeof(server_addr);

        pthread_t receiveThread, sendThread;
        // socket creation
        if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror("socket creation failed");
                exit(1);
        }
        printf("socket creation successful\n");

        // address set up
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.01", &server_addr.sin_addr);

        // connection
        if((connect(client_socket, (struct sockaddr*)&server_addr, server_len)) == -1)
        {
                perror("connection failed");
                exit(1);
        }
        sprintf(msg, "----- %s has joined the chat-----\n", name);
        write(client_socket, msg, sizeof(msg));

        // thread creation
        pthread_create(&receiveThread, NULL, receiveMsg, NULL);
        pthread_create(&sendThread, NULL, sendMsg, NULL);

        // joins
        pthread_join(receiveThread, NULL);
        pthread_join(sendThread, NULL);

        // closing
        close(client_socket);
}

void *receiveMsg(void *arg)
{
        while(1)
        {
                memset(buffer, 0, BUFFER_SIZE);

                recv(client_socket, buffer, BUFFER_SIZE, 0);
                printf("%s", buffer);

                if(strcasecmp(buffer, "exit\n") == 0)
                {
                        printf("Server Exiting...\nClosing the chat...\n");
                        break;
                }
        }
}

void *sendMsg(void *arg)
{
        while(1)
        {
                fgets(msg, MSG_SIZE, stdin);
                sprintf(buffer, "%s: %s", name, msg);
                send(client_socket, buffer, BUFFER_SIZE, 0);

                if(strcasecmp(msg, "exit\n") == 0)
                {
                        printf("%s exiting...\n", name);
                        break;
                }
        }
}


/*

SAMPLE OUTPUT:

socket creation successful
----- tony has joined the chat-----
----- steve has joined the chat-----
----- natasha has joined the chat-----
so we are all here
tony: so we are all here
natasha: yeah, cap what's your call
steve: we have to split
natasha: sure
your plan?
tony: your plan?
steve: 3 stone, 3 avengers, 1 shot
roger that
tony: roger that
natasha: copy that
exit
tony exiting...
tony: exit
natasha: exit
steve: good luck guys
yes
steve: bye bye
steve: exit
^C

socket creation successful
----- steve has joined the chat-----
----- natasha has joined the chat-----
tony: so we are all here
natasha: yeah, cap what's your call
we have to split
steve: we have to split
natasha: sure
tony: your plan?
3 stone, 3 avengers, 1 shot
steve: 3 stone, 3 avengers, 1 shot
tony: roger that
natasha: copy that
tony: exit
natasha: exit
good luck guys
steve: good luck guys
bye bye
steve: bye bye
exit
steve exiting...
steve: exit
exit
^C

socket creation successful
----- natasha has joined the chat-----
tony: so we are all here
yeah, cap what's your call
natasha: yeah, cap what's your call
steve: we have to split
sure
natasha: sure
tony: your plan?
steve: 3 stone, 3 avengers, 1 shot
tony: roger that
copy that
natasha: copy that
tony: exit
exit
natasha exiting...
natasha: exit
steve: good luck guys
you too cap
steve: bye bye
steve: exit
^C
*/