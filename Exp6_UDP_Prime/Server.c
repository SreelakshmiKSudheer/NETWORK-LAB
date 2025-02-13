#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX 100
#define PORT 6000

int primeF(int n,int factor[],int *f)
{
	int i,j, prime = 1, limit = n/2;

	factor[(*f)++] = 1;
	for(i = 2; i <= limit; i++)
	{
		if(n%i == 0)
		{
			prime = 0;
			factor[(*f)++] = i;
		}
	}
	factor[(*f)++] = n;
	return prime;
}

int main()
{
	int serverSocket;
	char buffer[MAX];
	struct sockaddr_in serverAddr,clientAddr;
	socklen_t addrSize;
	int num, f, i;

	serverSocket = socket(AF_INET,SOCK_DGRAM,0);
	if(serverSocket < 0)
	{
		printf("Socket creation failed");
		return -1;
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(bind(serverSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr)) < 0)
	{
		printf("Bind failed");
		close(serverSocket);
		return -1;
	}

	printf("Server is listening\n");
	addrSize = sizeof(clientAddr);
	while(1)
	{
		memset(buffer,0,sizeof(buffer));
		int recvLen = recvfrom(serverSocket,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&clientAddr,&addrSize);
                if(recvLen>0)
                {
                        buffer[recvLen]='\0';
                        if(strcmp(buffer,"exit")==0)
                        {
                                printf("Client exited.\n");
				printf("Server exiting.\n");
                                break;
                        }
			num = atoi(buffer);
			f = 0;
			int factor[50];
			printf("Message from client: %s\n",buffer);
			//printf("%d\n",num);
			int p = primeF(num, factor, &f);
			if(p)
			{
				printf("Prime\n");
				strcpy(buffer,"Prime\n");
			}
			else
			{
				printf("Composite\nFactors: ");
				strcpy(buffer,"Composite\nFactors: ");
				for(i = 0; i < f; i++)
				{
					printf("%d ",factor[i]);
					char str[10];
					sprintf(str,"%d",factor[i]);
					strcat(buffer,str);
					strcat(buffer," ");
				}
				printf("\n");
			}
                	buffer[strlen(buffer)]='\0';
                	sendto(serverSocket,buffer,strlen(buffer),0,(struct sockaddr*)&clientAddr,addrSize);

         	}
	}
	close(serverSocket);
	return 0;
}

