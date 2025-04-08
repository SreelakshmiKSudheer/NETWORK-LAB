#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>

#define BUFFER_SIZE 20
#define FRAMES 7

int main(int argc, char *argv[])
{
        int server_socket, client_socket;

        struct sockaddr_in server_addr, client_addr;
        socklen_t client_len = sizeof(client_addr);

        if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror("Socket creation failed");
                exit(1);
        }
        printf("Socket creation successful\n");

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(atoi(argv[1]));
        server_addr.sin_addr.s_addr = INADDR_ANY;

        if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
                perror("Socket binding failed");
                exit(1);
        }
        printf("Socket binding successful\n");

        if(listen(server_socket, 5) == -1)
        {
                perror("Listening failed");
                exit(1);
        }
        printf("Server listening\n");

        if((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len)) == -1)
        {
                perror("Accepting failed");
                exit(1);
        }
        printf("Connected to Client\n");

        int ack, expected_seq = 0, rec_seq;
        char buffer[BUFFER_SIZE];

        while(expected_seq < FRAMES)
        {
                memset(buffer, 0, BUFFER_SIZE);
                if(recv(client_socket, &rec_seq, sizeof(int), 0) > 0)
                {
                        //rec_seq = atoi(buffer);
                        printf("Received frame %d\n", rec_seq);
                        if(rec_seq < expected_seq)
                        {
                                printf("Duplicate frame received. Expecting frame %d\n", expected_seq);
                                send(client_socket, &expected_seq, sizeof(int), 0);
                                printf("Sent ACK %d\n", expected_seq);
                        }
                        else if(rec_seq > expected_seq)
                        {
                                printf("Out-of-Order frame received. Still expecting frame %d\n", expected_seq);
                                send(client_socket, &expected_seq, sizeof(int), 0);
                                printf("Sent ACK %d\n", expected_seq);
                        }
                        else
                        {
                                // simulating ack lost
                                if(rand() % 100 < 20)
                                {
                                        printf("Simulating ACK lost for ACK %d\n", rec_seq +1);
                                        expected_seq = rec_seq + 1;
                                }
                                else
                                {
                                        ack = rec_seq + 1;
                                        send(client_socket, &ack, sizeof(int),0);
                                        printf("Sent ACK %d\n", ack);
                                        expected_seq = ack;

                                }
                        }
                }
        }

        close(client_socket);
        close(server_socket);

        return 0;
}

/*
Socket creation successful
Socket binding successful
Server listening
Connected to Client
Received frame 0
Sent ACK 1
Received frame 1
Sent ACK 2
Received frame 2
Sent ACK 3
Received frame 3
Simulating ACK lost for ACK 4
Received frame 4
Sent ACK 5
Received frame 5
Sent ACK 6
Received frame 6
Sent ACK 7
*/