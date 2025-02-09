#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>

#define MAX 100
#define PORT 3028
int main()
{
	int serverSocket;
	char buffer[MAX];
	struct sockaddr_in serverAddr,clientAddr;
	socklen_t addrSize;

	serverSocket = socket(AF_INET,SOCK_DGRAM,0);
	if(serverSocket<0)
	{
		printf("Socket creation failed");
		return -1;
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(bind(serverSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0)
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
                        printf("Message from Client: %s\n",buffer);
                        if(strcmp(buffer,"exit")==0)
                        {
                                printf("Client exited.\n");
                                continue;
                        }
			memset(buffer,0,sizeof(buffer));
			printf("Enter a message: ");
                	fgets(buffer,sizeof(buffer),stdin);
                	buffer[strcspn(buffer,"\n")]='\0';
                	sendto(serverSocket,buffer,strlen(buffer),0,(struct sockaddr*)&clientAddr,addrSize);

                	if(strcmp(buffer,"exit")==0)
                	{
                        	printf("Server shutting down\n");
                        	break;
                	}
         	}
	}
	close(serverSocket);
	return 0;
}

