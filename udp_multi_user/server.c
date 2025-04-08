#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 7860
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

struct sockaddr_in clients[MAX_CLIENTS];
int client_count = 0;

int main() {
    int server_socket;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d...\n", PORT);

    while (1) {

        int bytes_received = recvfrom(server_socket, buffer, BUFFER_SIZE, 0,(struct sockaddr *)&client_addr, &addr_len);
        buffer[bytes_received] = '\0';
        printf("Received from %s:%d - %s\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port), buffer);

        int i;
        for (i = 0; i < client_count; i++) {
            if (clients[i].sin_addr.s_addr == client_addr.sin_addr.s_addr &&
                clients[i].sin_port == client_addr.sin_port) {
                break;
            }
        }
        if (i == client_count && client_count < MAX_CLIENTS) {
            clients[client_count++] = client_addr;
        }
        for (i = 0; i < client_count; i++) {
            if (clients[i].sin_addr.s_addr != client_addr.sin_addr.s_addr ||
                clients[i].sin_port != client_addr.sin_port) {
                sendto(server_socket, buffer, strlen(buffer), 0, 
                       (struct sockaddr *)&clients[i], addr_len);
            }
        }
    }

    close(server_socket);
    return 0;
}
