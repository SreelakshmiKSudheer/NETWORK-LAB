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
#define TIMEOUT 3

int main(int argc, char *argv[])
{
    int client_socket;

    struct sockaddr_in server_addr, client_addr;
    socklen_t server_len = sizeof(server_addr);

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }
    printf("Socket creation successful\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if ((connect(client_socket, (struct sockaddr *)&server_addr, server_len)) == -1)
    {
        perror("Connection failed");
        exit(1);
    }
    printf("Connection successful\n");

    int next_seq = 0, ack, total = 0, count = 0;
    int acked[FRAMES], sent[FRAMES], i, j;
    time_t timers[FRAMES];
    for (i = 0; i < FRAMES; i++)
    {
        acked[i] = 0;
        sent[i] = 0;
        timers[i] = 0;
    }

    struct timeval timeout;
    fd_set readfds;
    srand(time(0));

    while (total < FRAMES)
    {
        for (i = 0; i < FRAMES; i++)
        {
            if (!acked[i] && !sent[i])
            {
                for (j = 0; j < i + WINDOW_SIZE && j < FRAMES; j++)
                {
                    if (!acked[j] && !sent[j])
                    {
                        if (rand() % 100 < 20)
                        {
                            printf("Simulating frame lost for seq %d\n", j);
                        }
                        else
                        {
                            send(client_socket, &j, sizeof(int), 0);
                            printf("Sending frame %d\n", j);
                        }
                        sent[j] = 1;
                        timers[j] = time(NULL);
                    }
                }
                break;
            }
        }

        for(j = 0; j < FRAMES; j++)
        {
            if(sent[j] && !acked[j])
            {
                if(difftime(time(NULL), timers[j]) >= TIMEOUT)
                {
                    printf("Timeout.. Resending frame %d\n", j);
                    send(client_socket, &j, sizeof(int), 0);
                    timers[j] = time(NULL);
                }
            }
        }

        FD_ZERO(&readfds);
        FD_SET(client_socket, &readfds);
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        if (select(client_socket + 1, &readfds, NULL, NULL, &timeout) > 0)
        {
            // memset(buffer, 0, BUFFER_SIZE);
            if ((recv(client_socket, &ack, sizeof(int), 0)) > 0)
            {
                // ack = atoi(buffer);
                printf("Received ACK %d\n", ack);
                if (ack > 0 && ack <= FRAMES)
                {
                    acked[ack - 1] = 1;
                    total++;
                }
            }
        }
    }

    close(client_socket);
    return 0;
}

/*

Socket creation successful
Connection successful
Simulating frame lost for seq 0
Sending frame 1
Sending frame 2
Simulating frame lost for seq 3
Sending frame 4
Received ACK 3
Sending frame 5
Sending frame 6
Received ACK 5
Received ACK 7
Timeout.. Resending frame 0
Timeout.. Resending frame 1
Timeout.. Resending frame 3
Timeout.. Resending frame 5
Received ACK 1
Received ACK 2
Received ACK 4
Timeout.. Resending frame 5
Received ACK 6

*/