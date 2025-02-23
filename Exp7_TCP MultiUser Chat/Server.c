#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_CLIENTS 20
#define MAX 1024

void* serve_cl(void *arg);
void broadcast_msg(char *msg, int len);
void handle_err(char *err_msg);

int cl_cnt = 0;
int cl_socks[MAX_CLIENTS];
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
    int sv_sock, cl_sock;
    struct sockaddr_in sv_addr, cl_addr;
    socklen_t cl_addr_sz;
    pthread_t t_id;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutex, NULL);

    sv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sv_sock == -1)
        handle_err("ERROR: socket() failed");

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(sv_sock, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) == -1)
        handle_err("ERROR: bind() failed");

    if (listen(sv_sock, 5) == -1)
        handle_err("ERROR: listen() failed");

    printf("Group chat server is running on port: %s\n", argv[1]);

    while (1)
    {
        cl_addr_sz = sizeof(cl_addr);
        cl_sock = accept(sv_sock, (struct sockaddr *)&cl_addr, &cl_addr_sz);
        if (cl_sock == -1)
            continue;

        pthread_mutex_lock(&mutex);
        cl_socks[cl_cnt++] = cl_sock;
        pthread_mutex_unlock(&mutex);

        pthread_create(&t_id, NULL, serve_cl, (void *)&cl_sock);
        pthread_detach(t_id);

        printf("Connected client IP: %s\n", inet_ntoa(cl_addr.sin_addr));
    }

    close(sv_sock);
    return 0;
}

void* serve_cl(void *arg)
{
    int cl_sock = *(int*)arg;
    int str_len = 0, i;
    char msg[MAX];

    while ((str_len = read(cl_sock, msg, sizeof(msg))) > 0)
    {
        msg[str_len] = '\0';
        broadcast_msg(msg, str_len);
    }

    pthread_mutex_lock(&mutex);

    for (i = 0; i < cl_cnt; i++)
    {
        if (cl_socks[i] == cl_sock)
        {
            while (i < cl_cnt - 1)
            {
                cl_socks[i] = cl_socks[i + 1];
                i++;
            }
            break;
        }
    }

    cl_cnt--;
    pthread_mutex_unlock(&mutex);

    close(cl_sock);
    return NULL;
}

void broadcast_msg(char *msg, int len)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < cl_cnt; i++)
    {
        write(cl_socks[i], msg, len);
    }
    pthread_mutex_unlock(&mutex);
}

void handle_err(char *err_msg)
{
    perror(err_msg);
    exit(1);
}
