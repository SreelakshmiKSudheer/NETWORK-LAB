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
    int base = 0, next_seq = 0, ack, i, j, next_exp = 0;
    int packet_buffer[WINDOW_SIZE];  // Buffer to hold sent packets until ACKed
    int lost_packet = -1;

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

    // Main loop to send packets
    for (j = 0; j < TOTAL_PACKETS; j++)
    {
        // Send packets within the window size
        for (i = 0; i < WINDOW_SIZE; i++)
        {
            if (next_seq < TOTAL_PACKETS)
            {
                if (rand() % 4 != 0) // Simulate packet loss with a probability (1 in 4)
                {
                    // Send packet in order
                    send(client_socket, &next_seq, sizeof(next_seq), 0);
                    printf("Sent packet with seq: %d\n", next_seq);
                    packet_buffer[next_seq % WINDOW_SIZE] = next_seq;
                    next_seq++;
                    lost_packet = -1;
                }
                else
                {
                    // Simulate packet loss
                    printf("Packet seq %d lost! Will resend on timeout.\n", next_seq);
                    lost_packet = next_seq;
                }
            }
        }

        // Receive ACK from server
        if (recv(client_socket, &ack, sizeof(ack), 0) > 0)
        {
            printf("Received ACK: %d\n", ack);

            // If the ACK is not equal to the expected packet
            if (ack == next_exp)
            {
                printf("ACK matches, window advances.\n");
                next_exp++;
                base = ack;
            }
            else
            {
                // Resend the packet whose ACK was not received correctly
                printf("ACK mismatch! Resending packet %d\n", ack - 1);
                send(client_socket, &ack - 1, sizeof(ack), 0);
            }

            // Handle packet loss by resending the lost packet
            if (lost_packet != -1)
            {
                send(client_socket, &lost_packet, sizeof(lost_packet), 0);
                printf("Resending lost packet with seq: %d\n", lost_packet);
                lost_packet = -1;
            }
        }
    }

    close(client_socket);
    return 0;
}