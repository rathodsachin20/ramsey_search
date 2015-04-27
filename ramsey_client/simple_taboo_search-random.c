#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "graph_utils.h"
#include "fifo.h"	/* for taboo list */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include "connect.h"
#define MAXSIZE (541)

#define TABOOSIZE (500)
#define BIGCOUNT (9999999)

/***
 *** example of very simple search for R(7,7) counter examples
 ***
 *** starts with a small randomized graph and works its way up to successively
 *** larger graphs one at a time
 ***
 *** uses a taboo list of size #TABOOSIZE# to hold and encoding of and edge
 *** (i,j)+clique_count
 ***/

/*
 * PrintGraph
 *
 * prints in the right format for the read routine
 */
void PrintGraph(int *g, int gsize)
{
	int i;
	int j;

	fprintf(stdout,"%d\n",gsize);

	for(i=0; i < gsize; i++)
	{
		for(j=0; j < gsize; j++)
		{
			fprintf(stdout,"%d ",g[i*gsize+j]);
		}
		fprintf(stdout,"\n");
	}

	return;
}

/*
 * CopyGraph 
 *
 * copys the contents of old_g to corresponding locations in new_g
 * leaving other locations in new_g alone
 * that is
 * 	new_g[i,j] = old_g[i,j]
 */
void CopyGraph(int *old_g, int o_gsize, int *new_g, int n_gsize)
{
	int i;
	int j;

	/*
	 * new g must be bigger
	 */
	if(n_gsize < o_gsize)
		return;

	for(i=0; i < o_gsize; i++)
	{
		for(j=0; j < o_gsize; j++)
		{
			new_g[i*n_gsize+j] = old_g[i*o_gsize+j];
		}
	}

	return;
}


/*
 ***
 *** returns the number of monochromatic cliques in the graph presented to
 *** it
 ***
 *** graph is stored in row-major order
 *** only checks values above diagonal
 */

int CliqueCount(int *g,
		int gsize)
{
	int i;
	int j;
	int k;
	int l;
	int m;
	int n;
	int o;
	int count=0;
	int sgsize = 7;

	for(i=0;i < gsize-sgsize+1; i++)
	{
		for(j=i+1;j < gsize-sgsize+2; j++)
		{
			for(k=j+1;k < gsize-sgsize+3; k++) 
			{ 
				if((g[i*gsize+j] == g[i*gsize+k]) && 
						(g[i*gsize+j] == g[j*gsize+k]))
				{
					for(l=k+1;l < gsize-sgsize+4; l++) 
					{ 
						if((g[i*gsize+j] == g[i*gsize+l]) && 
								(g[i*gsize+j] == g[j*gsize+l]) && 
								(g[i*gsize+j] == g[k*gsize+l]))
						{
							for(m=l+1;m < gsize-sgsize+5; m++) 
							{
								if((g[i*gsize+j] == g[i*gsize+m]) && 
										(g[i*gsize+j] == g[j*gsize+m]) &&
										(g[i*gsize+j] == g[k*gsize+m]) && 
										(g[i*gsize+j] == g[l*gsize+m])) {
									for(n=m+1; n < gsize-sgsize+6; n++)
									{
										if((g[i*gsize+j]
													== g[i*gsize+n]) &&
												(g[i*gsize+j] 
												 == g[j*gsize+n]) &&
												(g[i*gsize+j] 
												 == g[k*gsize+n]) &&
												(g[i*gsize+j] 
												 == g[l*gsize+n]) &&
												(g[i*gsize+j] 
												 == g[m*gsize+n])) {
											for(o=n+1; o < gsize-sgsize+7; o++) {
												if((g[i*gsize+j]
															== g[i*gsize+o]) &&
														(g[i*gsize+j] 
														 == g[j*gsize+o]) &&
														(g[i*gsize+j] 
														 == g[k*gsize+o]) &&
														(g[i*gsize+j] 
														 == g[l*gsize+o]) &&
														(g[i*gsize+j] 
														 == g[m*gsize+o]) &&
														(g[i*gsize+j] == 
														 g[n*gsize+o])) {
													count++;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return(count);
}

#if 0
int open_socket(){
	int sock;
	struct sockaddr_in server;
	char message[1000] , server_reply[2000];

	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");

	server.sin_addr.s_addr = inet_addr("169.231.19.110");
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
	if( write(sock, &gsize, sizeof(gsize) ) < 0 ) 
		error("ERROR writing to socket");
	/*int *gCopy = (int*)malloc(gsize*gsize*sizeof(int));
	memcpy(gCopy,g,gsize*gsize*sizeof(int));
	struct sockaddr_in sockInfo;
	char *toSend = (char*)malloc(255+(gsize*gsize+3)*sizeof(char));
	int i,j;
	toSend[0] = gsize;
	for(i=1;i<gsize*gsize+1;i++)
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
	free(toSend);*/
}
#endif

int main(int argc,char *argv[])
{
	int *g;
	int *new_g;
	int gsize;
	int count;
	int i;
	int j;
	int best_count;
	int best_i;
	int best_j;
	void *taboo_list;
	int val,iter,jter;

	int sock = open_socket();
	/*
	 * start with graph of size 40
	 */
	gsize = 40;
	g = (int *)malloc(gsize*gsize*sizeof(int));
	if(g == NULL) {
		exit(1);
	}

	/*
	 * make a fifo to use as the taboo list
	 */
	taboo_list = FIFOInitEdge(TABOOSIZE);
	if(taboo_list == NULL) {
		exit(1);
	}

	/*
	 * start out with all zeros
	 */
	memset(g,0,gsize*gsize*sizeof(int));
	val = 0, iter = 0, jter=0;
	for( iter=0; iter<gsize; iter++){
		for( jter = 0; jter< gsize; jter++){
			g[iter*gsize + jter]  = val;
			val = 1 - val; 
		}
	}
	PrintGraph(g, gsize);

	int flag = 0;
	/*
	 * while we do not have a publishable result
	 */
	while(gsize < 204)
	{
		/*
		 * find out how we are doing
		 */
		count = CliqueCount(g,gsize);

		/*
		 * if we have a counter example
		 */
		if(count == 0)
		{
			printf("Eureka!  Counter-example found!\n");
			PrintGraph(g,gsize);
			//socket_upload(sock, g, gsize);	
			/*
			 * make a new graph one size bigger
			 */
			new_g = (int *)malloc((gsize+1)*(gsize+1)*sizeof(int));
			if(new_g == NULL)
				exit(1);
			/*
			 * copy the old graph into the new graph leaving the
			 * last row and last column alone
			 */
			//CopyGraph(g,gsize,new_g,gsize+1);
			new_g = RandomGraphGenerator(gsize+1);

			/*
			 * zero out the last column and last row
			 */
			/*for(i=0; i < (gsize+1); i++)
			  {
			  if(drand48() > 0.5) {
			  new_g[i*(gsize+1) + gsize] = 0; // last column
			  new_g[gsize*(gsize+1) + i] = 0; // last row
			  }
			  else
			  {
			  new_g[i*(gsize+1) + gsize] = 1; // last column
			  new_g[gsize*(gsize+1) + i] = 1; // last row
			  }
			  }*/

			/*
			 * throw away the old graph and make new one the
			 * graph
			 */
			free(g);
			g = new_g;
			gsize = gsize+1;

			/*
			 * reset the taboo list for the new graph
			 */
			taboo_list = FIFOResetEdge(taboo_list);

			/*
			 * keep going
			 */
			continue;
		}

		/*
		 * otherwise, we need to consider flipping an edge
		 *
		 * let's speculative flip each edge, record the new count,
		 * and unflip the edge.  We'll then remember the best flip and
		 * keep it next time around
		 *
		 * only need to work with upper triangle of matrix =>
		 * notice the indices
		 */
		for(i=0; i < gsize; i++)
		{
			for(j=i+1; j < gsize; j++)
			{
				/*
				 * flip it
				 */
				g[i*gsize+j] = 1 - g[i*gsize+j];
				count = CliqueCount(g,gsize);

				/*
				 * is it better and the i,j,count not taboo?
				 */
				if((count < best_count) && 
						!FIFOFindEdge(taboo_list,i,j))
					//					!FIFOFindEdgeCount(taboo_list,i,j,count))
				{
					best_count = count;
					best_i = i;
					best_j = j;
				}

				/*
				 * flip it back
				 */
				g[i*gsize+j] = 1 - g[i*gsize+j];
			}
		}

		if(best_count == BIGCOUNT) {
			printf("no best edge found, terminating\n");
			exit(1);
		}

		/*
		 * keep the best flip we saw
		 */
		g[best_i*gsize+best_j] = 1 - g[best_i*gsize+best_j];

		/*
		 * taboo this graph configuration so that we don't visit
		 * it again
		 */
		count = CliqueCount(g,gsize);
		FIFOInsertEdge(taboo_list,best_i,best_j);
		//		FIFOInsertEdgeCount(taboo_list,best_i,best_j,count);

		printf("ce size: %d, best_count: %d, best edge: (%d,%d), new color: %d\n",
				gsize,
				best_count,
				best_i,
				best_j,
				g[best_i*gsize+best_j]);

		/*
		 * rinse and repeat
		 */
	}

	FIFODeleteGraph(taboo_list);


	return(0);

}

int *RandomGraphGenerator(int n)
{
	int *g = (int*)malloc(n*n*sizeof(int));
	int i=0,j=0;
	srand(time(0));
	for(i=0;i<n;i++)
	{
		for(j=i+1;j<n;j++)
			g[i*n+j] = rand()%2;
	}
	return g;
}

