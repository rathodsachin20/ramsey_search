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

	printf("Uploading graph \n");
	char *key;
	MakeGraphKey(g,gsize,&key);

	char buf[1024];// =  (char *)malloc(1024 * sizeof(char));
	bzero(buf, 1024);
	sprintf(buf, "{ 'gsize' : %d , 'graph': %s}",gsize, key);
	
	int n = write(sock,&buf,sizeof(buf));

	if(n < 0)
		printf("Write failed \n");

	printf("sent.\n");
	free(key);
	
}



void socket_upload_2(int sock, int x, int y)
{
	if(sock == -1)
		return;

	char buf[1024];
	bzero(buf, 1024);
	sprintf(buf, "fifo_update: { 'x' : %d , 'y': %d}",x, y);

	int n = write(sock,&buf,sizeof(buf));

	if(n < 0)
		printf("Write failed \n");

	printf("sent.\n");
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

