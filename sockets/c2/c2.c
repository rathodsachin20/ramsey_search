//client side socket functions
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define BUFFSIZE 512

//write the graph to file
int write_to_file(int data, char *fname){
  FILE* fp;
  fp = fopen(fname,"w");
  if(!fp){
    printf("oops");
        fprintf(stderr,"Cannot open file\n");
        return 0;
    }
  fprintf(fp,"1");
  fclose(fp);
  return 1;
}

//send the graph to server
int c_gsend(char *fname){
  int sockfd=0;
  char sendBuff[BUFFSIZE];
  struct sockaddr_in serv_addr;
  char buff[BUFFSIZE];

  char send_ip[32]="127.0.0.1";

  memset(sendBuff, '0', sizeof(sendBuff));
  memset(&serv_addr, '0', sizeof(serv_addr));

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0){
    printf("\n Error: Could not create socket \n");
    return 0;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5000);

  if(inet_pton(AF_INET, send_ip, &serv_addr.sin_addr)<=0){
    printf("\n inet_pton error occured \n");
    return 0;
  }

  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
    printf("\n Error: Connect failed \n");
    return 0;
  }

  FILE *fp;
  fp = fopen(fname,"r");
  if(!fp){
    printf("Error opening file\n");
    return 0;
  }

  while(1){
    char buff[BUFFSIZE]={0};
    int nread = fread(buff,1,BUFFSIZE,fp);
    printf("Bytes read %d\n",nread);

    if(nread > 0){
      printf("Sending....\n");
      write(sockfd, buff, nread);
    }

    if(nread < BUFFSIZE){
      if(feof(fp))
        printf("End of file\n");
      if(ferror(fp))
        printf("error reading\n");
      break;
    }
  }
  
  puts("im here");
  bzero(buff,256);
  
  int n = read(sockfd, buff, sizeof(buff)-1);
  printf("%s",buff);
  close(sockfd);
  return 1;
}

int recvack(){
  return 0;
}

//receive graph from server
int c_grecv(){
  printf("waiting.....");
  return 0;
}

int main(int argc, char *argv[]){
  int i=0;
  for(i=1; i<11; i++)
    if(i%10==0){
      char fname[32];
      snprintf(fname,32,"%d",i);
      printf("%s",fname);
      if(!write_to_file(i,fname)){
        printf("write to file failed for fname: %d",i);
        break;
      }
      if(!c_gsend(fname)){
        printf("sending failed for fname: %d",i);
        break;
      }
    }
  return 0;
}
