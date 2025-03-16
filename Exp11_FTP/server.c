#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SIZE 1024

void writeFile(int clientSocket) {
    char buffer[SIZE];
    char receivedData[SIZE * 10] = "";  // Buffer to store full received data

    FILE* fp = fopen("output.txt", "w");
    if (fp == NULL) {
        perror("File opening error");
        exit(EXIT_FAILURE);
    }

    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';  // Ensure null termination
        strcat(receivedData, buffer);  // Store received data
    }

    if (bytesReceived < 0) {
        perror("Receive error");
    }

    // Reverse the received data
    int len = strlen(receivedData);
    for (int i = 0, j = len - 1; i < j; i++, j--) {
        char temp = receivedData[i];
        receivedData[i] = receivedData[j];
        receivedData[j] = temp;
    }

    // Write reversed content to file
    fprintf(fp, "%s", receivedData);
    printf("Reversed content:\n%s\n", receivedData);

    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd, newsockfd;
    struct sockaddr_in servaddr, clientaddr;
    socklen_t clilen = sizeof(clientaddr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) < 0) {
        perror("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %s...\n", argv[1]);

    newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &clilen);
    if (newsockfd < 0) {
        perror("Accept failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    writeFile(newsockfd);

    close(newsockfd);
    close(sockfd);
    return 0;
}
