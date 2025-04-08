#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define SIZE 2048

int client_socket;

void writeFile()
{
        char buffer[SIZE], reverse[SIZE];
        FILE *fp;

        if((fp = fopen("output", "w")) == NULL)
        {
                printf("Couldn't open file\n");
                exit(1);
        }

        recv(client_socket, buffer, SIZE, 0);

        int i, len = strlen(buffer);
        for(i = 0; i < len; i++)
                reverse[i] = buffer[len-1-i];
        fprintf(fp, "%s", reverse);

        printf("Reversed content: \n%s\n", reverse);

        fclose(fp);
}


int main(int argc, char *argv[])
{
        int server_socket;
        struct sockaddr_in server_addr,client_addr;
        socklen_t client_len = sizeof(client_addr);

        if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror("Socket creation failed");
                exit(1);
        }
        printf("Socket creation successful\n");

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(atoi(argv[1]));
        server_addr.sin_addr.s_addr = INADDR_ANY;

        if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
                perror("Socket binding failed");
                exit(1);
        }
        printf("Socket binding successful\n");

        if(listen(server_socket, 5) == -1)
        {
                perror("Listening failed");
                exit(1);
        }
        printf("Server Listening\n");

        if((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len)) == -1)
        {
                perror("Accept failed");
                exit(1);
        }
        printf("Accepted client\n");

        writeFile();

        close(client_socket);
        close(server_socket);
}