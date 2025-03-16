#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define SIZE 1024

void readFile(int clientSocket) {
    char filename[100];
    printf("Enter Filename to send: ");
    scanf("%s", filename);

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char buffer[SIZE];
    int bytesRead;

    while ((bytesRead = fread(buffer, sizeof(char), SIZE - 1, fp)) > 0) {
        buffer[bytesRead] = '\0';  // Ensure null termination
        if (send(clientSocket, buffer, bytesRead, 0) < 0) {
            perror("Error in sending");
            close(clientSocket);
            fclose(fp);
            return;
        }
    }

    printf("%s sent successfully\n", filename);
    fclose(fp);
    close(clientSocket);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));

    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    readFile(sockfd);
    close(sockfd);

    return 0;
}
