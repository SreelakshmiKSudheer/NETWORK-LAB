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
#define PORT 3520
#define SA struct sockaddr

void func(int sockfd)
{
	char buff[MAX];
	int n;
	while(1)
	{
		bzero(buff,MAX);
		printf("Enter a string: ");
		n = 0;
		while((buff[n++] = getchar()) != '\n');
		write(sockfd, buff, sizeof(buff));
		bzero(buff,sizeof(buff));
		read(sockfd,buff,sizeof(buff));
		printf("From Server: %s\n ",buff);
		if(strncmp("exit",buff,4) == 0)
		{
			printf("Client Exit\n");
			break;
		}
	}
}

int main()
{
	int sockfd, connfd;
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
		if(connect(sockfd,(SA*)&servaddr, sizeof(servaddr)) != 0)
		{
			printf("Connection failed\n");
			exit(0);
		}
		else
		{
			printf("Connection successful\n");
			func(sockfd);
			close(connfd);
		}

	}
}

