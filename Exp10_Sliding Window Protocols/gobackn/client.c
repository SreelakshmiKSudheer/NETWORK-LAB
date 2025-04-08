#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>

#define BUFFER_SIZE 20
#define WINDOW_SIZE 5
#define FRAMES 7

int main(int argc, char *argv[])
{
        int client_socket;

        struct sockaddr_in server_addr, client_addr;
        socklen_t server_len = sizeof(server_addr);

        if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror("Socket creation failed");
                exit(1);
        }
        printf("Socket creation successful\n");

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(atoi(argv[1]));
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        if((connect(client_socket, (struct sockaddr*)&server_addr, server_len)) == -1)
        {
                perror("Connection failed");
                exit(1);
        }
        printf("Connection successful\n");

        int next_seq = 0, ack, base = 0;
        char buffer[BUFFER_SIZE];
        int acked[FRAMES], i;
        for(i = 0; i<FRAMES; i++)
                acked[i] = 0;

        struct timeval timeout;
        fd_set readfds;
        srand(time(0));

        while(base < FRAMES)
        {
                while(next_seq <= base+WINDOW_SIZE-1 && next_seq < FRAMES)
                {
                        // frame loss
                        if(rand() % 100 < 20)
                        {
                                printf("Simulating frame lost for seq %d\n", next_seq);
                                next_seq++;
                                //continue;
                        }
                        else
                        {
                                // sending frame
//                              memset(buffer, 0, BUFFER_SIZE);
//                              sprintf(buffer, "%d", next_seq);
                                send(client_socket, &next_seq, sizeof(int), 0);
                                printf("Sending frame %d\n", next_seq);
                                next_seq++;
                        }
                        /*
                        FD_ZERO(&readfds);
                        FD_SET(client_socket, &readfds);
                        timeout.tv_sec = 3;
                        timeout.tv_usec = 0;

                        */
                }
                FD_ZERO(&readfds);
                FD_SET(client_socket, &readfds);
                timeout.tv_sec = 3;
                timeout.tv_usec = 0;

                if(select(client_socket + 1, &readfds, NULL, NULL, &timeout) == 0)
                {
                        printf("Timeout..., Resend from frame %d\n", base);
                        next_seq = base;
                }
                else
                {
                        //memset(buffer, 0, BUFFER_SIZE);
                        if((recv(client_socket, &ack, sizeof(int), 0)) > 0)
                        {
                                //ack = atoi(buffer);
                                printf("Received ACK %d\n", ack);
                                if(ack > 0)
                                        acked[ack-1] = 1;
                                if(ack > base)
                                        base = ack;
                        }
                }
                for(i = 0; i < FRAMES; i++)
                        if(acked[i] == 0)
                                break;
                base = i;
        }

        close(client_socket);
        return 0;

}


/*

Socket creation successful
Connection successful
Sending frame 0
Sending frame 1
Sending frame 2
Sending frame 3
Sending frame 4
Received ACK 1
Sending frame 5
Received ACK 2
Sending frame 6
Received ACK 3
Received ACK 5
Received ACK 6
Received ACK 7
*/