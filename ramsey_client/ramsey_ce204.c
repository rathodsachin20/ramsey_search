#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "fifo.h"	/* for taboo list */
//#include "graph_utils.h"

#define USE_TABOO

#define MAXSIZE (541)

#define TABOOSIZE (1000)
#define BIGCOUNT (9999999999)

int* g_latest = NULL;
int g_size_latest = 1;
long g_count_latest = BIGCOUNT;


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
void PrintGraph(int *g, int gsize,FILE *fp, char *key)
{
	int i;
	int j;

	fprintf(fp,"%d",gsize);
	fprintf(fp," %s\n",key);
	for(i=0; i < gsize; i++)
	{
		for(j=0; j < gsize; j++)
		{
			fprintf(fp,"%d ",g[i*gsize+j]);
		}
		fprintf(fp,"\n");
	}

	return;
}
/*
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
*/
void sig_handler(int signum){
	printf("Caught signal %d\n", signum);
	//char filename[100] = "dump_";
	//char str_num[32];
	//sprintf(str_num, "%d.%d", g_size_latest, g_count_latest);
	//strcat(filename, str_num);
	if(g_latest != NULL){
		//printf("Printing current graph with size: %d and count: %ld\n", g_size_latest, g_count_latest);
		//PrintGraph(g_latest, g_size_latest);
		fflush(stdout);
	}
	else {
		printf("No graph to print :(\n");
	}
	if(signum == SIGINT){
		printf("Continuing execution.\n");
		fflush(stdout);
		return;
	}
	else if (signum == SIGTERM){
		printf("Exiting.\n");
		exit(signum);
		fflush(stdout);
	}
}



/*
 * reads a graph of the correct format from the file referenced
 * by #fname#.  The resulting is passed back through the #g# and #gsize#
 * out parameters.  The space for the file is malloced and the routine
 * is not thread safe.  Returns 1 on success and 0 on failure.
 */
	int
ReadGraph(char *fname,
		int **g,
		int *gsize)
{
	int i;
	int j;
	FILE *fd;
	int lsize;
	int *lg;
	char line_buff[511];
	char *curr;
	char *err;
	char *tempc;
	int lcount;

	fd = fopen(fname,"r");
	if(fd == 0)
	{
		fprintf(stderr,"ReadGraph cannot open file %s\n",fname);
		fflush(stderr);
		return(0);
	}

	fgets(line_buff,510,fd);
	if(feof(fd))
	{
		fprintf(stderr,"ReadGraph eof on size\n");
		fflush(stderr);
		fclose(fd);
		return(0);
	}	
	i = 0;
	while((i < 510) && !isdigit(line_buff[i]))
		i++;

	/*
	 * first line of the file must contain a size
	 */
	if(!isdigit(line_buff[i]))
	{
		fprintf(stderr,"ReadGraph format error on size\n");
		fflush(stderr);
		fclose(fd);
		return(0);
	}
	tempc = line_buff;
	lsize = (int)strtol(tempc,&tempc,10);
	if((lsize < 0) || (lsize > MAXSIZE))
	{
		fprintf(stderr,"ReadGraph size bad, read: %d, max: %d\n",
				lsize,MAXSIZE);
		fflush(stderr);
		fclose(fd);
		return(0);
	}

	lg = (int *)malloc(lsize*lsize*sizeof(int));
	if(lg == NULL)
	{
		fprintf(stderr,"ReadGraph: no space\n");
		fflush(stderr);
		return(0);
	}

	memset(lg,0,lsize*lsize*sizeof(int));

	for(i=0; i < lsize; i++)
	{
		if(feof(fd))
		{
			break;
		}
		err = fgets(line_buff,510,fd);
		if(err == NULL)
		{
			break;
		}
		curr = line_buff;
		for(j=0; j < lsize; j++)
		{
			sscanf(curr,"%d ",&(lg[i*lsize+j]));
			if((lg[i*lsize+j] != 1) && 
					(lg[i*lsize+j] != 0))
			{
				fprintf(stderr,
						"ReadGraph: non-boolean value read: %d\n", 
						lg[i*lsize+j]);
				fflush(stderr);
				fclose(fd);
				return(0);
			}
			while(isdigit(*curr))
				curr++;
			while(isspace(*curr))
				curr++;
		}
	}

	if(i < lsize)
	{
		fprintf(stderr,"ReadGraph file too short, lsize: %d\n",lsize);
		fflush(stderr);
		fclose(fd);
		return(0);
	}

	fclose(fd);

	*g = lg;
	*gsize = lsize;
	return(1);
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

long CliqueCount(int *g,
	     int gsize)
{
    int i;
    int j;
    int k;
    int l;
    int m;
    int n;
    int o;
    long count=0;
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

int getRandomJ(int gsize) {

	int k = 0;
	srand(time(NULL));
	return(rand() % (gsize - 1));

}


int* PaleyGraph(gsize){
	int i, k;
        int *g = (int * )malloc(gsize * gsize * sizeof(int));
        memset(g, 0, gsize * gsize);

        for(i=0; i<gsize; i++){
                for(k= 1; k< (gsize)/ 2; k++)
                {
                        int v1 = i;
                        int v2 = ( i + k*k ) % gsize;
                        if(v1 > v2)
                        {
                                int j = v1;
                                v1 = v2;
                                v2 = j;
                        }
                        g[v1*gsize + v2] = 1;
                        g[v2*gsize + v1] = 1;
                }
        }

        return g;

}



int* lbgraph(n){
	int * g = PaleyGraph(n);
	int gsize = n+n+2;
	int* newg = (int*) malloc (gsize*gsize*sizeof(int));
	int i, j;
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			newg[i*gsize + j] = newg[j*gsize+i] = g[i*n+j];
			newg[(i+n)*gsize + (j+n)] = g[i*n+j];
			if(g[i*n+j]==0){
				newg[i*gsize+(j+n)] = 1;
			}
		}
	}
	int v1 = gsize-2;
	for(i=0; i<n; i++){
		newg[i*gsize+v1] = 1;
	}
	int v2 = gsize-1;
	for(i=n; i<2*n; i++){
		newg[i*gsize+v2] = 1;
	}
	return newg;
}



int
main(int argc,char *argv[])
{
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);

	int *new_g;
	int gsize;
	long count;
	int i;
	int j;
	int val,iter,jter;
	char fname[255];
	FILE *fp;

	if (argc < 2) {
		printf("usage: %s <paley graph size> [ex. %s 101 for R(7,7)]\n", argv[0], argv[0]);
		fflush(stdout);

	} else if (argc == 2) {

		gsize = atoi(argv[1]);
		//g = (int *)malloc(gsize*gsize*sizeof(int));
		//if(g == NULL) {
		//	exit(1);
        	//}
		//g = PaleyGraph(gsize);
		printf("Starting from Paley graph of size %d\n.", gsize);
		int* newg = lbgraph(gsize);
		gsize = gsize*2 + 2;
		count = CliqueCount(newg,gsize);
		if(count == 0)
			printf("Eureka!  Counter-example found! size:%d\n", gsize);
		sprintf(fname,"r-%d.txt",gsize);
		fp = fopen(fname,"w");
		printf("writing graph obtained by to a file: %s\n", fname);
		char *key;
		(void)MakeGraphKey(newg,gsize,&key);
	        PrintGraph(newg,gsize,fp, key);    
       		fclose(fp);
		free(key);
		//PrintGraph(newg, gsize*2+2);
		fflush(stdout);
		free(newg);
		return 0;
	}
	else {
		printf("usage: %s <paley graph size> [ex. %s 101 for R(7,7)]\n", argv[0], argv[0]);
	}

	return(0);

}
