#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>

#define SIZE 20

int client_socket;

int main(int argc, char *argv[])
{
        // sockets, port
        int server_socket;
        int port = atoi(argv[1]);

        int expected_seq = 0, ack, rec_seq;
        char buffer[20];

        struct sockaddr_in server_addr,client_addr;
        socklen_t client_len = sizeof(client_addr);
        struct timeval timeout;
        fd_set readfds;

        srand(time(NULL));

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
        while(expected_seq < 5)
        {
                memset(buffer, 0, SIZE);
                if(recv(client_socket, buffer, SIZE, 0) > 0)
                {
                        rec_seq = atoi(buffer);
                        printf("Received seq %d\n", rec_seq);

                        if(rec_seq == expected_seq)
                        {
                                // simulate ack loss
                                if(rand() % 100 < 30)
                                {
                                        printf("Simulating ack lost for frame %d", rec_seq);
                                        continue;
                                }
                                memset(buffer, 0, SIZE);
                                ack = rec_seq + 1;
                                sprintf(buffer, "%d", ack);
                                send(client_socket, buffer, SIZE, 0);
                                printf("Sent ACK %d", ack);
                                expected_seq++;
                        }
                }
        }
        close(client_socket);
        close(server_socket);
        return 0;
}

/*
cc Server.c -o server.out
./server.out 3004

SAMPLE OUTPUT:

Socket creation successful
Socket binding successful
Server listening...
Connected to client
Received seq 0
Simulating ack lost for frame 0
Received seq 0
Sent ACK 1
Received seq 1
Simulating ack lost for frame 1
Received seq 1
Simulating ack lost for frame 1
Received seq 1
Simulating ack lost for frame 1
Received seq 1
Sent ACK 2
Received seq 2
Sent ACK 3
Received seq 3
Simulating ack lost for frame 3
Received seq 3
Sent ACK 4
Received seq 4
Sent ACK 5
*/