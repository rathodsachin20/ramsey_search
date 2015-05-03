#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "fifo.h"	/* for taboo list */
//#include "graph_utils.h"

#define USE_TABOO

#define MAXSIZE (541)

#define TABOOSIZE (2000)
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





int
main(int argc,char *argv[])
{
	int *g;
	int *new_g;
	int gsize;
	int count;
	int count_1;
	int count_2;
	int count_3;
	int i;
	int j;
	int best_count;
	int best_i;
	int best_j;
	int best_k;
	int best_l;
	void *taboo_list;
	int val,iter,jter;
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
	}
	else {
		gsize = atoi(argv[1]);
		g = (int *)malloc(gsize*gsize*sizeof(int));
		ReadGraph(argv[2], &g, &gsize);
		printf("Starting from given graph of size %d\n.", gsize);
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
	while(gsize < 206)
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
					best_count = count;
					if(count == count_1
//#ifdef USE_TABOO
						&& !FIFOFindEdgeCount(taboo_list,i,j,count)
//#endif
						)
					{
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

		/*
		 * rinse and repeat
		 */
	}

	FIFODeleteGraph(taboo_list);


	return(0);

}
