#include<stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

int open_socket(const char* address){
	int sock;
	struct sockaddr_in server;
	char message[1000] , server_reply[2000];

	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");

	server.sin_addr.s_addr = inet_addr(address);
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	puts("Connected\n");
	return sock;
}

void socket_upload(int sock, int *g,int gsize)
{
	if(sock == -1)
		return;
	
	printf("Uploading graph");
	int *gCopy = (int*)malloc(gsize*gsize*sizeof(int));
	memcpy(gCopy,g,gsize*gsize*sizeof(int));
	struct sockaddr_in sockInfo;
	char *toSend = (char*)malloc(255+(gsize*gsize+2)*sizeof(char));
	int i,j;
	unsigned char bytes[4];
	bytes[0] = (gsize >> 24) & 0xFF;
	bytes[1] = (gsize >> 16) & 0xFF;
	bytes[2] = (gsize >> 8) & 0xFF;
	bytes[3] = gsize & 0xFF;
	write(sock,&bytes,sizeof(bytes));
	
	for(i=0;i<gsize*gsize;i++)
	{
		toSend[i] = '0' + gCopy[i];
	}
	toSend[i] = '\n';
	toSend[i+1] = '\0';
	while(1)
	{
		for(j=0;j<(gsize*gsize+2)/255+1;j++)
		{
			if(send(sock,toSend+j*255,255,0)<0)
			{
				printf("fail, retrying at package %d...\n",j);
				j--;
				continue;
			}
		}
		printf("wait.\n");
		char buf[256];
		buf[0] = 0;
		int received = recv(sock,buf,1,0);
		if(received==1)
			break;
	}
	printf("sent.\n");
	free(gCopy);
	free(toSend);
}

