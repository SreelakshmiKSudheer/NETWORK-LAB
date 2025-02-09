#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#define MAX 100
#define PORT 3014
#define SA struct sockaddr

void func(int connfd)
{
	char buff[MAX];
	char msg[MAX];
	int n,i;
	while(1)
	{
		bzero(buff,MAX);
		read(connfd,buff, sizeof(buff));
		printf("From client: %s",buff);
		//bzero(buff,MAX);
		if(strncmp("exit",buff,4) == 0)
		{
			printf("Client Exited\n");
			break;
		}
		if(buff[0] >= 'a' && buff[0] <= 'z')
			buff[0] -= 32;
		for(i = 1; buff[i] != '\0'; i++)
		{
			if(buff[i] >= 'a' && buff[i] <= 'z' && buff[i-1] == ' ')
				buff[i] -= 32;
		}
		printf("%s\n",buff);
		write(connfd, buff, sizeof(buff));
	}
}

int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	
	if(sockfd == -1)
	{
		printf("Socket creation failed\n");
		exit(0);
	}
	else
	{
		printf("Socket creation successful\n");
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = inet_addr("127.0.0.20");
		servaddr.sin_port = htons(PORT);
		if((bind(sockfd,(SA*)&servaddr, sizeof(servaddr))) != 0)
		{
			printf("Socket binding failed\n");
			exit(0);
		}
		else
		{
			printf("Socket binding successful\n");
			if((listen(sockfd,5)) != 0)
			{
				printf("Listening failed\n");
				exit(0);
			}
			else
			{
				printf("Server listening\n");
				len = sizeof(cli);
				connfd = accept(sockfd,(SA*)&cli,&len);
				if(connfd < 0)
				{
					printf("Server accept failed\n");
					exit(0);
				}
				else
				{
					printf("Server accept the client\n");
					func(connfd);
					close(sockfd);
				}
			}
		}

	}
}


