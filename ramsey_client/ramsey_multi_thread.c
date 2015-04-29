#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "fifo.h"	/* for taboo list */


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

struct thread_args {

	void *taboo_list;
	int *g;
	int gsize;
};

#define NumThreads 2
pthread_mutex_t taboo_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t t2_wait = PTHREAD_COND_INITIALIZER;

void *thread_search(void *args);
void *thread_search2(void *args);
int t1_count = 0;
int t2_count = 0;
int flag[205] = {0};

int main(int argc,char *argv[])
{
	int *g;
	int gsize;
	int count;
	int i;
	int j;
	void *taboo_list;
	int val,iter,jter;
	pthread_t *pthreads = (pthread_t *)malloc(sizeof(pthread_t)* NumThreads);
	struct thread_args *args = (struct thread_args*) malloc(sizeof(struct thread_args));	
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

	} else {

		gsize = atoi(argv[1]);
		g = (int *)malloc(gsize*gsize*sizeof(int));
		if(g == NULL) {
			exit(1);
        	}
		g = PaleyGraph(gsize);
	}
	/*
	 *make a fifo to use as the taboo list
	 */
        taboo_list = FIFOInitEdge(TABOOSIZE);
        if(taboo_list == NULL) {
                exit(1);
        }
	/* populate thread parameters */
	args->g = g;
	args->gsize = gsize;
	args->taboo_list = taboo_list;

	pthread_create( &pthreads[0], NULL, thread_search, (void *)args);
	pthread_create( &pthreads[1], NULL, thread_search2, (void *)args);


	
	for (i = 0; i < NumThreads; i++) {
		pthread_join(pthreads[i], NULL);
	}
	/* join threads here */
	FIFODeleteGraph(taboo_list);
	return(0);
}

void *thread_search(void *args) {

	
	struct thread_args *arg = (struct thread_args *)args;
	int *new_g;
	int *g = arg->g;
	int gsize = arg->gsize;
	int count;
	int i;
	int j;
	int best_count;
	int best_i;
	int best_j;
	int best_k;
	void *taboo_list = arg->taboo_list;
	int iter,jter,val;
	printf("\n thread id is %u ", pthread_self());



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
			pthread_mutex_lock(&taboo_lock);
			while (flag[t2_count] == 0) {
				printf("\n struck here \n");
				pthread_cond_wait(&t2_wait, &taboo_lock);
			}
			t1_count++;
		//	taboo_list = FIFOResetEdge(taboo_list);
			pthread_mutex_unlock(&taboo_lock);
			printf("\n __func %s", __func__);
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
				g[i*gsize+j] = 1 - g[i*gsize+j];
				if (k == j)
					k = j + 1;
				g[i*gsize + k] = 1 - g[i*gsize + k];
				count = CliqueCount(g,gsize);

				/*
				 * is it better and the i,j,count not taboo?
				 */
				if((count < best_count) && 
//					!FIFOFindEdge(taboo_list,i,j))
					!FIFOFindEdgeCount(taboo_list,i,j,count) &&
					!FIFOFindEdgeCount(taboo_list,i,k, count))
				{
					/* no need to store j + 1 */
					best_count = count;
					best_i = i;
					best_j = j;
					best_k = k;
				}

				/*
				 * flip it back
				 */
				g[i*gsize+j] = 1 - g[i*gsize+j];
				g[i*gsize+k] = 1 - g[i*gsize+k];
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
		g[best_i*gsize + best_k] = 1 - g[best_i*gsize + best_k];

		/*
		 * taboo this graph configuration so that we don't visit
		 * it again
		 */
		count = CliqueCount(g,gsize);
//		FIFOInsertEdge(taboo_list,best_i,best_j);

		pthread_mutex_lock(&taboo_lock);
		FIFOInsertEdgeCount(taboo_list,best_i,best_j,count);
		FIFOInsertEdgeCount(taboo_list,best_i,best_k,count);
		pthread_mutex_unlock(&taboo_lock);
		printf("pthread_id %u ce size: %d, best_count: %d, best edges: (%d,%d) (%d,%d), new colors: %d %d\n",
			pthread_self(),
			gsize,
			best_count,
			best_i,
			best_j,
			best_i,
			best_k,
			g[best_i*gsize+best_j],
			g[best_i*gsize+best_k]);

		/*
		 * rinse and repeat
		 */
	}
}


void *thread_search2(void *args) {

	
	struct thread_args *arg = (struct thread_args *)args;
	int *new_g;
	int *g;
	int gsize = arg->gsize;
	int count;
	int i;
	int j;
	int best_count;
	int best_i;
	int best_j;
	int best_k;
	void *taboo_list = arg->taboo_list;
	int val;
	printf("\n thread id is %u ", pthread_self());

		gsize = 8;
		g = (int *)malloc(gsize*gsize*sizeof(int));
		if(g == NULL) {
			exit(1);
		}

	/*
	 * start out with all zeros
	 */
		memset(g,0,gsize*gsize*sizeof(int));
		val = 0, i = 0, j = 0;
		for( i=0; i<gsize; i++){
			for( j = 0; j< gsize; j++){
				g[i*gsize + j]  = val;
				val = 1 - val; 
			}
		}
		PrintGraph(g, gsize);
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
			pthread_mutex_lock(&taboo_lock);
			flag[t2_count++] = 1;
			taboo_list = FIFOResetEdge(taboo_list);
			pthread_cond_signal(&t2_wait);
			pthread_mutex_unlock(&taboo_lock);
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
				g[i*gsize+j] = 1 - g[i*gsize+j];
				if (k == j)
					k = j + 1;
				g[i*gsize + k] = 1 - g[i*gsize + k];
				count = CliqueCount(g,gsize);

				/*
				 * is it better and the i,j,count not taboo?
				 */
				if((count < best_count) && 
//					!FIFOFindEdge(taboo_list,i,j))
					!FIFOFindEdgeCount(taboo_list,i,j,count) &&
					!FIFOFindEdgeCount(taboo_list,i,k, count))
				{
					/* no need to store j + 1 */
					best_count = count;
					best_i = i;
					best_j = j;
					best_k = k;
				}

				/*
				 * flip it back
				 */
				g[i*gsize+j] = 1 - g[i*gsize+j];
				g[i*gsize+k] = 1 - g[i*gsize+k];
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
		g[best_i*gsize + best_k] = 1 - g[best_i*gsize + best_k];

		/*
		 * taboo this graph configuration so that we don't visit
		 * it again
		 */
		count = CliqueCount(g,gsize);
//		FIFOInsertEdge(taboo_list,best_i,best_j);
		pthread_mutex_lock(&taboo_lock);
		FIFOInsertEdgeCount(taboo_list,best_i,best_j,count);
		FIFOInsertEdgeCount(taboo_list,best_i,best_k,count);
		pthread_mutex_unlock(&taboo_lock);
		printf("pthread id %u ce size: %d, best_count: %d, best edges: (%d,%d) (%d,%d), new colors: %d %d\n",
			pthread_self(),
			gsize,
			best_count,
			best_i,
			best_j,
			best_i,
			best_k,
			g[best_i*gsize+best_j],
			g[best_i*gsize+best_k]);

		/*
		 * rinse and repeat
		 */
	}
}
