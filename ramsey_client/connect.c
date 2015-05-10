#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include<stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
 #include <sys/ioctl.h>
#include <ifaddrs.h>

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
	
	printf("Uploading graph - 1");

	int *gCopy = (int*)malloc(gsize*gsize*sizeof(int));
	memcpy(gCopy,g,gsize*gsize*sizeof(int));
	
	struct sockaddr_in sockInfo;
	
	char *toSend = (char*)malloc(255+(gsize*gsize+2)*sizeof(char));
	int i,j;

	char *key;
	MakeGraphKey(g,gsize,&key);
	
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
	free(key);
	free(toSend);
	
}

/*
 * prints in the right format for the read routine
 */
void PrintGraphToFile(int *g, int gsize, FILE * fout)
{
	if(fout == NULL)
		fout = stdout;

	int i;
	int j;

	fprintf(fout,"%d\n",gsize);

	for(i=0; i < gsize; i++)
	{
		for(j=0; j < gsize; j++)
		{
			fprintf(fout,"%d ",g[i*gsize+j]);
		}
		fprintf(fout,"\n");
	}

	return;
}

void socket_upload_2(int sock, int x, int y, int count, int gsize, int *g)
{
	if(sock == -1)
		return;

	printf("Uploading graph -2 \n");
	char *key;
	MakeGraphKey(g,gsize,&key);

	char buf[1024];
	bzero(buf, 1024);
	sprintf(buf, "{ 'x' : %d , 'y': %d, 'count': %d, 'gsize' : %d , 'graph': '%s' }", x, y, count, gsize, key);
	int n = write(sock,&buf,sizeof(buf));

	printf("Sending graph \n");
 	socket_upload(sock, g,gsize);

	if(n < 0)
		printf("Write failed \n");

	bzero(buf, 1024);
	read(sock, buf, 1024);
	printf("%s\n", buf);
	int len = 0;

	if( strstr(buf, "graph") !=NULL ){
		printf("updating...\n");
		for(int i=0; i<5 || len > 1024; i++)
		{
			bzero(buf, 1024);
			len = read(sock, buf, 1024);
			printf("Len is: %d\n", len);
			printf("Buf is: %s\n", buf);
		}
	}
	
	printf("sent.\n");
	free(key);
}

void* wait_for_server(void *arg)
{
	int sock = *((int *) arg);

    printf("Starting server YAYYAYAYAYAYAY\n");
	if(sock == -1)
		return 0;

    printf("Starting server YAYYAYAYAYAYAY\n");

    int read_size;
    char *message , client_message[2000];

    printf("Starting server YAYYAYAYAYAYAY\n");
    while(1)
     {
     	//Receive a message from client
	    while( (read_size = read(sock , client_message , 2000 , 0)) > 0 )
	    {
	        //end of string marker
			client_message[read_size] = '\0';
			
			//Send the message back to client
			printf("Recieved !!!!! %s",client_message);
			
			//clear the message buffer
			memset(client_message, 0, 2000);
	    }
	}
         
    return 0;
}

/*
* This function recieves messages from the master node and performs instructions accordingly
*/
void create_fifo_thread(int *sockPtr)
{
	pthread_t tid;
	int err = pthread_create(&tid, NULL, &wait_for_server, &sockPtr);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
    else
        printf("\n Thread created successfully\n");
}

