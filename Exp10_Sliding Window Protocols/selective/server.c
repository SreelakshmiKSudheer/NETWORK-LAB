#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define WINDOW_SIZE 4
#define TOTAL_PACKETS 10

int main(int argc, char *argv[])
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    int received[TOTAL_PACKETS] = {0};
    int seq, ack;

    if (argc < 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Binding failed");
        exit(1);
    }

    if (listen(server_socket, 1) == -1)
    {
        perror("Listening failed");
        exit(1);
    }

    printf("Waiting for client...\n");
    addr_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
    if (client_socket == -1)
    {
        perror("Connection failed");
        exit(1);
    }

    printf("Client connected.\n");

    while (1)
    {
        if (recv(client_socket, &seq, sizeof(seq), 0) > 0)
        {
            printf("Received packet with seq: %d\n", seq);

            if (seq < TOTAL_PACKETS && received[seq] == 0)
            {
                received[seq] = 1;
                ack = seq + 1;
                send(client_socket, &ack, sizeof(ack), 0);
                printf("Sent ACK: %d\n", ack);
            }
        }

        int done = 1;
        for (int i = 0; i < TOTAL_PACKETS; i++)
        {
            if (received[i] == 0)
            {
                done = 0;
                break;
            }
        }

        if (done)
        {
            printf("All packets received. Closing connection.\n");
            break;
        }
    }

    close(client_socket);
    close(server_socket);
    return 0;
}