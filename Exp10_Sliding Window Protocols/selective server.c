#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define WINDOW_SIZE 4
#define TOTAL_PACKETS 10

int main(int argc, char *argv[])
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    int received_seq, expected_seq = 0;
    int ack, window[WINDOW_SIZE] = {-1}; // Buffer for out-of-order packets

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
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

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

    printf("Waiting for connection...\n");
    addr_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
    if (client_socket == -1)
    {
        perror("Connection acceptance failed");
        exit(1);
    }
    printf("Client connected!\n");

    while (1)
    {
        if (recv(client_socket, &received_seq, sizeof(received_seq), 0) > 0)
        {
            printf("Received packet: %d\n", received_seq);

            if (received_seq == expected_seq) 
            {
                //printf("Delivering: %d\n", received_seq);
                expected_seq++;  // Move window forward
                
                // Check buffered packets for delivery
                while (window[expected_seq % WINDOW_SIZE] != -1)
                {
                  //  printf("Delivering buffered: %d\n", expected_seq);
                    window[expected_seq % WINDOW_SIZE] = -1;
                    expected_seq++;
                }
            }
            else if (received_seq > expected_seq && received_seq < expected_seq + WINDOW_SIZE)
            {
                printf("Out-of-order packet %d buffered\n", received_seq);
                window[received_seq % WINDOW_SIZE] = received_seq; // Store in buffer
            }
            else
            {
                printf("Packet %d ignored (out of window)\n", received_seq);
            }

            ack = received_seq + 1;
	    expected_seq = ack;
            send(client_socket, &ack, sizeof(ack), 0);
            printf("Sent ACK: %d\n", ack);
        }
        else
        {
            break; // Exit if connection is lost
        }
    }

    close(client_socket);
    close(server_socket);

    return 0;
}
