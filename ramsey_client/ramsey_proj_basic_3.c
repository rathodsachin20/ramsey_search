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

#define TABOOSIZE (10)
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

int* g_latest = NULL;
int* g_orig = NULL;
int g_size_latest = 1;
int g_count_latest = BIGCOUNT;
int count_orig = BIGCOUNT;


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

void sig_handler(int signum){
	printf("Caught signal %d\n", signum);
	//char filename[100] = "dump_";
	//char str_num[32];
	//sprintf(str_num, "%d.%d", g_size_latest, g_count_latest);
	//strcat(filename, str_num);
	if(g_latest != NULL){
		printf("Printing current graph with size: %d and count: %d\n", g_size_latest, g_count_latest);
		PrintGraph(g_latest, g_size_latest);
	}
	else {
		printf("No graph to print :(\n");
	}
	exit(signum);
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
	char line_buff[255];
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

	fgets(line_buff,254,fd);
	if(feof(fd))
	{
		fprintf(stderr,"ReadGraph eof on size\n");
		fflush(stderr);
		fclose(fd);
		return(0);
	}	
	i = 0;
	while((i < 254) && !isdigit(line_buff[i]))
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
		err = fgets(line_buff,254,fd);
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
                }
        }

        return g;

}


void create_sgraph(int** g, int gsize, int* row){
	int i;
	int j;
	for(i=0; i<gsize; i++){
		for(j=i; j<gsize; j++){
			(*g)[j*gsize+i] = (*g)[i*gsize+j] = row[(j-i+gsize)%gsize];
		}
	}
}

void flip_sedge(int**g, int gsize, int i){
	int k;
	int val = 1 - (*g)[i];
	for(k=0; k<gsize; k++){
		(*g)[(i+k)%gsize+k*gsize] = (*g)[k*gsize+(i+k)%gsize] = val;
	}
}

void set_sedge(int**g, int gsize, int i, int val){
	int k;
	for(k=0; k<gsize; k++){
		(*g)[((i+k)%gsize)+k*gsize] = (*g)[(k*gsize)+(i+k)%gsize] = val;
	}
}

void get_good_row(int* g, int** row, int gsize, int rowsize){
	int i, j, count0, count1, diff;
	int best_row = 0;
	int best_diff = gsize;
	int best_count0 = 0;
	for(i=0; i<gsize; i++){
		count0 = count1 = 0;
		for(j=0; j<gsize; j++){
			if(g[i*gsize+j]==0)
				count0++;
		}
		count1 = gsize - count0;
		diff = count0>count1 ? count0-count1 : count1-count0;
		if(diff < best_diff){
			best_count0 = count0;
			best_diff = diff;
			best_row = i;
		}
	}
	printf("best row at:%d\n", best_row);
	memcpy(*row, g+i*best_row, gsize*sizeof(int));
	if(rowsize > gsize){ //Need to fill last position
		if(best_count0 >= gsize-best_count0) // more 0s than 1's
			row[gsize] = 1;
		else
			row[gsize] = 0;
	}
	return;
}
int
main(int argc,char *argv[])
{
	signal(SIGINT, sig_handler);
	
	int *g;
	int *new_g;
	int gsize;
	int count;
	int count_1;
	int count_2;
	int count_3;
	int i;
	int j;
	int k;
	int best_count;
	int prev_best_count;
	int best_i;
	int best_j;
	int best_k;
	int best_l;
	void *taboo_list;
	int* taboo_array;
	int val,iter,jter;
	int multi = 0;
	/*
	 * start with graph of size 8
	 */

	if (argc < 2) {
		gsize = 8;
		g = (int *)malloc(gsize*gsize*sizeof(int));
		if(g == NULL) {
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

	} else if (argc == 2) {

		gsize = atoi(argv[1]);
		g = (int *)malloc(gsize*gsize*sizeof(int));
		if(g == NULL) {
			exit(1);
        	}
		g = PaleyGraph(gsize);
		//int* row = (int *)malloc(gsize*sizeof(int));
		//memcpy(row, g, gsize*sizeof(int));
		//create_sgraph(&g, gsize, row);
		taboo_array = (int*)malloc(sizeof(int)*gsize);
		memset(taboo_array, 0, gsize*sizeof(int));
		printf("Starting from Paley graph of size %d\n.", gsize);
		fflush(stdout);
		//free(row);
	}
	else {
		gsize = atoi(argv[1]);
		g = (int *)malloc(gsize*gsize*sizeof(int));
		ReadGraph(argv[2], &g, &gsize);
		count = CliqueCount(g,gsize);
		if (count == 0)
		{
			printf("Eureka!  Sym Counter-example found!\n");
			PrintGraph(g,gsize);
			fflush(stdout);
			
			new_g = (int *)malloc((gsize+1)*(gsize+1)*sizeof(int));
			if(new_g == NULL)
				exit(1);
			
			int* row = (int *)malloc((gsize+1)*sizeof(int));
			get_good_row(g, &row, gsize, gsize+1);
			g_orig = g;
			count_orig = CliqueCount(g, gsize);
			//free(g);
			gsize = gsize+1;
			create_sgraph(&new_g, gsize, row);
			free(row);
			//CopyGraph(g,gsize,new_g,gsize+1);
			set_sedge(&new_g, gsize, gsize-1, 0);
			int count0  = CliqueCount(new_g, gsize);
			set_sedge(&new_g, gsize, gsize-1, 1);
			int count1  = CliqueCount(new_g, gsize);
			if(count0 < count1)
				set_sedge(&new_g, gsize, gsize-1, 0);
			taboo_array = (int*) malloc(sizeof(int)*gsize);
			memset(taboo_array, 0, sizeof(int)*gsize);
			g = new_g;
			
		}
		else
		{
			int* row = (int *)malloc(gsize*sizeof(int));
			get_good_row(g, &row, gsize, gsize);
			//memcpy(row, g, gsize*sizeof(int));
			create_sgraph(&g, gsize, row);
			taboo_array = (int*)malloc(sizeof(int)*gsize);
			memset(taboo_array, 0, gsize*sizeof(int));
			printf("Starting from given graph of size %d\n.", gsize);
			fflush(stdout);
			free(row);
		}
	}

	/*
	 *make a fifo to use as the taboo list
	 */
        taboo_list = FIFOInitEdge(TABOOSIZE);
        if(taboo_list == NULL) {
                exit(1);
        }


	/*
	 * while we do not have a publishable result
	 */
  while(gsize<206)
  {

	best_count = BIGCOUNT;
	while(gsize < 206)
	{
		best_j = -1;
		count = CliqueCount(g,gsize);
		if(count == 0)
		{
			printf("Eureka!  Sym Counter-example found!\n");
			PrintGraph(g,gsize);
			fflush(stdout);
			/*
			 * make a new graph one size bigger
			 */
			new_g = (int *)malloc((gsize+1)*(gsize+1)*sizeof(int));
			if(new_g == NULL)
				exit(1);
			
			int* row = (int *)malloc((gsize+1)*sizeof(int));
			get_good_row(g, &row, gsize, gsize+1);
			//memcpy(row, g, gsize*sizeof(int));
			free(g);
			gsize = gsize+1;
			create_sgraph(&new_g, gsize, row);
			free(row);
			//CopyGraph(g,gsize,new_g,gsize+1);
			set_sedge(&new_g, gsize, gsize-1, 0);
			int count0  = CliqueCount(new_g, gsize);
			set_sedge(&new_g, gsize, gsize-1, 1);
			int count1  = CliqueCount(new_g, gsize);
			if(count0 < count1)
				set_sedge(&new_g, gsize, gsize-1, 0);
			taboo_array = (int*) malloc(sizeof(int)*gsize);
			memset(taboo_array, 0, sizeof(int)*gsize);
			g = new_g;

			best_count = BIGCOUNT;
			/*
			 * reset the taboo list for the new graph
			 */
			//taboo_list = FIFOResetEdge(taboo_list);
			free(taboo_array);
			taboo_array = (int*) malloc(sizeof(int)*gsize);
			memset(taboo_array, 0, sizeof(int)*gsize);

			continue;
		}
		

		//best_count = BIGCOUNT;
		prev_best_count = best_count;
		if(!multi){
				for(i=0; i < gsize; i++)
				{
					
					flip_sedge(&g, gsize, i);
					count = CliqueCount(g,gsize);
					if(count<best_count && !taboo_array[i]){
					//if(count<best_count ){
						best_count = count;
						best_i = i;
					}
					flip_sedge(&g, gsize, i);
				}
		}
		else {
			best_j = best_k = -1;
			for(i=0; i<gsize; i++) {
				for(j=i+1; j<gsize; j++) {
					for(k=j+1; k<gsize; k++) {
						flip_sedge(&g, gsize, i);
						count_1 = CliqueCount(g,gsize);
						flip_sedge(&g, gsize, j);
						count_2 = CliqueCount(g,gsize);
						flip_sedge(&g, gsize, k);
						count_3 = CliqueCount(g,gsize);

                                		count = (count_1 < count_2) ? count_1 : count_2 ;
						count = (count_3 < count) ? count_3 : count ;
						if(count<best_count){
							best_count = count;
							if(count == count_1){
								best_i = i;
								best_j = -1;
								best_k = -1;
							}
							else if(count == count_2){
								best_i = i;
								best_j = j;
								best_k = -1;
							}
							else if(count == count_3){
								best_i = i;
								best_j = j;
								best_k = k;
							}

						}
						flip_sedge(&g, gsize, i);
						flip_sedge(&g, gsize, j);
						flip_sedge(&g, gsize, k);
					}
				}
			}
		}

		if(best_count == BIGCOUNT || best_count==prev_best_count) {
			if(multi)
			{
				printf("no best edge found, continuing with taboo\n");
				if(count_orig < best_count) {
					free(g);
					g = g_orig;
				}
				else {
					free(g_orig);
				}
				fflush(stdout);
				break;
			}
			else
			{
				printf("single flip exhausted. starting multi-flip.", multi);
				fflush(stdout);
				//memset(taboo_array, 0, sizeof(int)*gsize);
				multi = 1;
				continue;
			}
			//flip_sedge(g, gsize, i);
		}
		flip_sedge(&g, gsize, best_i);
		count = best_count;
		if(best_j != -1)
			flip_sedge(&g, gsize, best_j);
		if(best_k != -1)
			flip_sedge(&g, gsize, best_k);
		taboo_array[best_i] = 1;
		printf("sym ce size: %d, best_count: %d, best edge(s): (%d), (%d), (%d)\n", gsize, best_count, best_i, best_j, best_k);
		g_latest = g;
		g_size_latest = gsize;
		g_count_latest = count;
		fflush(stdout);
	}

	while(gsize < 206)
	{
		/*
		 * if we have a counter example
		 */
		if(count == 0)
		{
			printf("Eureka!  Counter-example found!\n");
			PrintGraph(g,gsize);
			fflush(stdout);
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
			CopyGraph(g,gsize,new_g,gsize+1);

			/*
			 * zero out the last column and last row
			 */
			for(i=0; i < (gsize+1); i++)
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
			}

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
			//continue;
			break; //Go to first while loop
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
		best_count = BIGCOUNT;
		for(i=0; i < gsize; i++)
		{
			for(j=i+1; j < gsize; j++)
			{
				/*
				 * flip two edges (i,j), (i,random(j) + 1) 
				 */
				int k = getRandomJ(gsize);
				int l = getRandomJ(gsize);

				g[i*gsize+j] = 1 - g[i*gsize+j];
				count_1 = CliqueCount(g,gsize);

				if (k == j)
					k = j + 1;
				g[i*gsize + k] = 1 - g[i*gsize + k];
				count_2 = CliqueCount(g,gsize);

				if (l == j)
					l = j + 1;
				if (l == k)
					l = (k + 1) % gsize;

				g[i*gsize + l] = 1 - g[i*gsize + l];
				count_3 = CliqueCount(g,gsize);

                                count = (count_1 < count_2) ? count_1 : count_2 ;
				count = (count_3 < count) ? count_3 : count ;

				/*
				 * is it better and the i,j,count not taboo?
				 */
				if(count < best_count){
					if(count == count_1
//#ifdef USE_TABOO
						&& !FIFOFindEdgeCount(taboo_list,i,j,count)
//#endif
						)
					{
						best_count = count;
						best_i = i;
						best_j = j;
						best_k = best_l = -1;
					}
					else if(count == count_2
//#ifdef USE_TABOO
						&& (!FIFOFindEdgeCount(taboo_list,i,j,count)
						|| !FIFOFindEdgeCount(taboo_list,i,k, count))
//#endif
						)
					{
						best_count = count;
						best_i = i;
						best_j = j;
						best_k = k;
						best_l = -1;
					}
					else if(count == count_3
//#ifdef USE_TABOO
						&& (!FIFOFindEdgeCount(taboo_list,i,j,count)
						|| !FIFOFindEdgeCount(taboo_list,i,k, count)
						|| !FIFOFindEdgeCount(taboo_list,i,l, count))
//#endif
						)
					{
						best_count = count;
						best_i = i;
						best_j = j;
						best_k = k;
						best_l = l;
					}
				} 

				/*
				 * flip it back
				 */
				g[i*gsize+j] = 1 - g[i*gsize+j];
				g[i*gsize+k] = 1 - g[i*gsize+k];
				g[i*gsize+l] = 1 - g[i*gsize+l];
			}
		}

		if(best_count == BIGCOUNT) {
			printf("no best edge found, terminating\n");
			exit(1);
		}
		
		/*
		 * keep the best flip we saw
		 */
		g[best_i*gsize + best_j] = 1 - g[best_i*gsize + best_j];
		if (best_k != -1)
			g[best_i*gsize + best_k] = 1 - g[best_i*gsize + best_k];
		if (best_l != -1)
			g[best_i*gsize + best_l] = 1 - g[best_i*gsize + best_l];

		/*
		 * taboo this graph configuration so that we don't visit
		 * it again
		 */
		//count = CliqueCount(g,gsize);
		count = best_count;
//#ifdef USE_TABOO
//		FIFOInsertEdge(taboo_list,best_i,best_j);
		FIFOInsertEdgeCount(taboo_list,best_i,best_j,count);
		if (best_k != -1)
			FIFOInsertEdgeCount(taboo_list,best_i,best_k,count);
		if (best_l != -1)
			FIFOInsertEdgeCount(taboo_list,best_i,best_l,count);
//#endif
		printf("ce size: %d, best_count: %d, best edges: (%d,%d) (%d,%d) (%d,%d), new colors: %d %d\n",
			gsize,
			best_count,
			best_i,
			best_j,
			best_i,
			best_k,
			best_i,
			best_l,
			g[best_i*gsize+best_j],
			g[best_i*gsize+best_k]);

		fflush(stdout);
		g_latest = g;
		g_size_latest = gsize;
		g_count_latest = count;
		/*
		 * rinse and repeat
		 */
	}
  }

	FIFODeleteGraph(taboo_list);


	return(0);

}
