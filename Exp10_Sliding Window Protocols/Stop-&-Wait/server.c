#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int seq, ack;
    struct timeval timeout = {3, 0};  // 3 seconds timeout

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

    if (listen(server_socket, 5) == -1)
    {
        perror("Listening failed");
        exit(1);
    }
    printf("Server is listening...\n");

    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket == -1)
    {
        perror("Accept failed");
        exit(1);
    }
    printf("Client connected.\n");

    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    ack = 0;
    while (ack <= 4)
    {
        if (recv(client_socket, &seq, sizeof(seq), 0) > 0)
        {
            printf("Received seq: %d\n", seq);
            if (seq == ack)
            {
                ack = seq + 1;
                send(client_socket, &ack, sizeof(ack), 0);
                printf("Sent ack: %d\n", ack);
            }
        }
        else
        {
            printf("Timeout! No packet received. Waiting...\n");
        }
    }

    close(client_socket);
    close(server_socket);

    return 0;
}
