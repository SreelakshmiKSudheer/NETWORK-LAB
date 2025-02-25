#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX 1024
#define NAME_LEN 1024

void *recv_msg(void *sock);
void *send_msg(void *sock);

char name[NAME_LEN] = "noname";
char msg[MAX];

int main(int argc, char *argv[])
{
    int cl_sock;
    struct sockaddr_in sv_addr;
    pthread_t recv_thread, send_thread;
    void *thread_return;

    if (argc != 4)
    {
        printf("Usage: %s <server IP> <port> <name>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "%s", argv[3]); // Fixed syntax

    cl_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (cl_sock == -1)
    {
        perror("ERROR: socket() failed");
        exit(1);
    }

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    sv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(cl_sock, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) == -1)
    {
        perror("ERROR: connect() failed");
        close(cl_sock);
        exit(1);
    }

    printf("Connected to group chat! (Type 'exit' to quit)\n\n");

    sprintf(msg, "----- %s has joined! -----\n", name);
    write(cl_sock, msg, strlen(msg)); // Fixed variable name

    pthread_create(&send_thread, NULL, send_msg, (void *)&cl_sock); // Fixed thread function
    pthread_create(&recv_thread, NULL, recv_msg, (void *)&cl_sock);

    pthread_join(send_thread, &thread_return);
    pthread_join(recv_thread, &thread_return);

    close(cl_sock);
    return 0;
}

void *send_msg(void *arg)
{
    int cl_sock = *(int *)arg;
    char full_msg[NAME_LEN + MAX];

    while (1)
    {
        fgets(msg, MAX, stdin);
        if (strcmp(msg, "exit\n") == 0)
        {
            sprintf(full_msg, "----- %s has left! -----\n", name);
            write(cl_sock, full_msg, strlen(full_msg));
            close(cl_sock);
            exit(0);
        }
        sprintf(full_msg, "%s: %s", name, msg);
        write(cl_sock, full_msg, strlen(full_msg));
    }

    return NULL;
}

void *recv_msg(void *sock)
{
    int cl_sock = *(int *)sock;
    char msg[MAX];
    int str_len;

    while ((str_len = read(cl_sock, msg, sizeof(msg) - 1)) > 0)
    {
        msg[str_len] = '\0';
        fputs(msg, stdout);
    }

    printf("Disconnected from server.\n");
    exit(0);
}
