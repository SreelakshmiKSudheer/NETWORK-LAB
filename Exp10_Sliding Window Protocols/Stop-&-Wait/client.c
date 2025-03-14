#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int client_socket;
    struct sockaddr_in server_addr;
    int seq = 0, ack;

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

    while (seq <= 4)
    {
        if (rand() % 3 != 0)  // Randomly drop packets (1 in 3 chance)
        {
            send(client_socket, &seq, sizeof(seq), 0);
            printf("Sent seq: %d\n", seq);
        }
        else
        {
            printf("Packet seq %d lost! Retrying...\n", seq);
            continue;  // Simulate retransmission
        }

        if (recv(client_socket, &ack, sizeof(ack), 0) > 0)
        {
            printf("Received ack: %d\n", ack);
            seq = ack;
        }
        else
        {
            printf("Timeout! Resending seq: %d\n", seq);
        }
    }

    close(client_socket);

    return 0;
}
