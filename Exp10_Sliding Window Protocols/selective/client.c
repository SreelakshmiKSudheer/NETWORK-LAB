#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define WINDOW_SIZE 4
#define TOTAL_PACKETS 10

int main(int argc, char *argv[])
{
    int client_socket;
    struct sockaddr_in server_addr;
    int base = 0, next_seq = 0, ack, i;
    int packet_status[TOTAL_PACKETS] = {0};

    if (argc < 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connection failed");
        exit(1);
    }
    printf("Connected to server.\n");

    srand(time(NULL));

    while (base < TOTAL_PACKETS)
    {
        for (i = 0; i < WINDOW_SIZE && next_seq < TOTAL_PACKETS; i++)
        {
            if (packet_status[next_seq] == 0)
            {
                if (rand() % 4 != 0)
                {
                    send(client_socket, &next_seq, sizeof(next_seq), 0);
                    printf("Sent packet with seq: %d\n", next_seq);
                }
                else
                {
                    printf("Packet seq %d lost! Will resend after timeout.\n", next_seq);
                }
            }
            next_seq++;
        }

        sleep(3);  // Increased timeout before resending

        if (recv(client_socket, &ack, sizeof(ack), 0) > 0)
        {
            printf("Received ACK: %d\n", ack);
            packet_status[ack] = 1; // Mark packet as received

            while (packet_status[base] == 1 && base < TOTAL_PACKETS)
                base++;
        }

        for (i = 0; i < TOTAL_PACKETS; i++)
        {
            if (packet_status[i] == 0)
            {
                send(client_socket, &i, sizeof(i), 0);
                printf("Resending packet with seq: %d\n", i);
            }
        }
    }

    close(client_socket);
    return 0;
}