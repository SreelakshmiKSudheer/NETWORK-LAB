#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>

#define SIZE 20

int main(int argc, char *argv[])
{
        int client_socket;
        int seq, ack;
        char buffer[20];

        struct sockaddr_in server_addr,client_addr;
        socklen_t server_len = sizeof(server_addr);
        struct timeval timeout;
        fd_set readfds;

        srand(time(NULL));

        if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror("Socket creation failed");
                exit(1);
        }
        printf("Socket creation successful\n");

        server_addr.sin_family = AF_INET;
        //server_addr,sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(atoi(argv[1]));
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        // connect
        if(connect(client_socket, (struct sockaddr*)&server_addr, server_len) == -1)
        {
                perror("Couldn't connect to server");
                exit(1);
        }
        printf("Connected to Server\n");

        // send frame
        for(seq = 0; seq < 5;)
        {
                if(rand() % 100 < 30)           // simulate frame loss
                {
                        printf("simulating frame loss for seq %d\n", seq);
                        sleep(1);
                        continue;
                }

                FD_ZERO(&readfds);
                FD_SET(client_socket, &readfds);
                timeout.tv_sec = 3;
                timeout.tv_usec = 0;

                memset(buffer, 0, SIZE);
                sprintf(buffer, "%d", seq);
                send(client_socket, buffer, strlen(buffer), 0);
                printf("Sent seq: %d\n", seq);

                if(select(client_socket+1, &readfds, NULL, NULL, &timeout) == 0)
                {
                        printf("Timeout, Resending frame seq %d\n", seq);
                        continue;
                }
                memset(buffer, 0, SIZE);
                if(recv(client_socket, buffer, SIZE, 0) > 0)
                {
                        ack = atoi(buffer);
                        printf("Received ACK %d\n", ack);
                        if(ack == seq+1)
                                seq++;
                }
        }
        close(client_socket);
        return 0;
}

/*
cc client.c -o client.out
./client.out 3004

SAMPLE OUTPUT:

Socket creation successful
Connected to Server
Sent seq: 0
Timeout, Resending frame seq 0
Sent seq: 0
Received ACK 1
Sent seq: 1
Timeout, Resending frame seq 1
Sent seq: 1
Timeout, Resending frame seq 1
Sent seq: 1
Timeout, Resending frame seq 1
Sent seq: 1
Received ACK 2
simulating frame loss for seq 2
Sent seq: 2
Received ACK 3
Sent seq: 3
Timeout, Resending frame seq 3
Sent seq: 3
Received ACK 4
Sent seq: 4
Received ACK 5

*/