#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define SIZE 2048

int client_socket;

void readFile()
{
        char filename[25], buffer[SIZE];
        int bytes;

        printf("Enter filename: ");
        scanf(" %[^\n]", filename);

        FILE *fp;

        if((fp = fopen(filename, "r")) == NULL)
        {
                printf("Couldn't openfile\n");
                exit(1);
        }
        while((bytes = fread(buffer, sizeof(char), SIZE-1, fp)) > 0)
        {
                buffer[bytes] = '\0';
                send(client_socket, buffer, SIZE, 0);
        }

        fclose(fp);
}


int main(int argc, char *argv[])
{
        struct sockaddr_in server_addr,client_addr;
        socklen_t server_len = sizeof(server_addr);

        if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror("Socket creation failed");
                exit(1);
        }
        printf("Socket creation successful\n");

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(atoi(argv[1]));
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        if(connect(client_socket, (struct sockaddr*)&server_addr, server_len) == -1)
        {
                perror("Connection failed");
                exit(1);
        }
        printf("Connection successful\n");

        readFile();

        close(client_socket);
}