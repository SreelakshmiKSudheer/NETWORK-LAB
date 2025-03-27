#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 7860
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    char buffer[BUFFER_SIZE];
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    while (1) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        strftime(buffer, BUFFER_SIZE, "Time: %Y-%m-%d %H:%M:%S", tm_info);
        sendto(client_socket, buffer, strlen(buffer), 0,(struct sockaddr *)&server_addr, addr_len);
        int bytes_received = recvfrom(client_socket, buffer, BUFFER_SIZE, 0,(struct sockaddr *)&server_addr, &addr_len);
        buffer[bytes_received] = '\0';
        printf("Received: %s\n", buffer);
        sleep(1);
    }

    close(client_socket);
    return 0;
}

