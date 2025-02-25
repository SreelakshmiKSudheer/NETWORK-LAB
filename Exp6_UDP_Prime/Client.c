#include <stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>

#define PORT 6000
#define MAX 100
int main()
{
	int clientSocket;
	char buffer[MAX];
	struct sockaddr_in serverAddr;
	socklen_t addrSize;

	clientSocket = socket(AF_INET,SOCK_DGRAM,0);
	if(clientSocket<0)
	{
		printf("Socket creation failed");
		return -1;
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	printf("Connected to server.\n");
	while(1)
	{
		memset(buffer,0,sizeof(buffer));
		printf("Enter a message: ");
		fgets(buffer,sizeof(buffer),stdin);
		buffer[strcspn(buffer,"\n")] = '\0';
		sendto(clientSocket,buffer,strlen(buffer),0,(struct sockaddr*)&serverAddr,sizeof(serverAddr));

		if(strcmp(buffer,"exit") == 0)
		{
			printf("Exiting client\n");
			break;
		}
		addrSize = sizeof(serverAddr);
		int recvLen = recvfrom(clientSocket,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&serverAddr,&addrSize);
		if(recvLen>0)
		{
			buffer[recvLen] = '\0';
			printf("Server: %s\n",buffer);
			/*if(strcmp(buffer,"exit") == 0)
			{
				printf("Server has exited.Closing client\n");
				break;
			}*/
		}
		else
                {
                       	printf("No respponse from server");
                        printf("Server might have exited.Closing client\n");
                        break;
                }
	}
	close(clientSocket);
        return 0;
}

