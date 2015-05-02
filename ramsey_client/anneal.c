#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "fifo.h"	/* for taboo list */
#include "graph_utils.h"
#include "connect.h"

#define sgsize 7
#define MAXSIZE (541)
#define TABOOSIZE (500)
#define BIGCOUNT (9999999)
#define THRESHOLD 50000 //temperature threshold
#define INITEM 100000//initial temperature
#define DTEM 100//temperature decrease interval
#define INICLI 9999999//initial best clinique value

int CliqueCount_D(int *g, int gsize, int i, int j, int flip) 
{
	int k;
	int l;
	int m;
	int n;
	int o;
	int count=0;
	int color = g[i*gsize+j];

	if(flip)
		color = 1 - color;

	for(k=0;k < gsize-sgsize+3; k++) 
	{ 
		if(k==i || k==j) continue;

		int k_array[2];
		if(i<k) k_array[0]=i*gsize+k;
		else 	k_array[0]=k*gsize+i;
		if(j<k) k_array[1]=j*gsize+k;
		else 	k_array[1]=k*gsize+j;

		if((color == g[k_array[0]]) && 
				(color == g[k_array[1]]))
		{
			for(l=k+1;l < gsize-sgsize+4; l++) 
			{
				if(l==i || l==j) continue;

				int l_array[2];
				if(i<l) l_array[0]=i*gsize+l;
				else	l_array[0]=l*gsize+i;
				if(j<l) l_array[1]=j*gsize+l;
				else	l_array[1]=l*gsize+j;

				if((color == g[l_array[0]]) && 
						(color == g[l_array[1]]) && 
						(color == g[k*gsize+l]))
				{
					for(m=l+1;m < gsize-sgsize+5; m++) 
					{
						if(m==i || m==j) continue;

						int m_array[2];
						if(i<m) m_array[0]=i*gsize+m;
						else	m_array[0]=m*gsize+i;
						if(j<m) m_array[1]=j*gsize+m;
						else	m_array[1]=m*gsize+j;


						if((color == g[m_array[0]]) && 
								(color == g[m_array[1]]) &&
								(color == g[k*gsize+m]) && 
								(color == g[l*gsize+m]))
						{
							for(n=m+1;n<gsize-sgsize+6;n++)
							{
								if(n==i || n==j) continue;

								int n_array[2];
								if(i<n) n_array[0]=i*gsize+n;
								else	n_array[0]=n*gsize+i;
								if(j<n) n_array[1]=j*gsize+n;
								else	n_array[1]=n*gsize+j;

								if((color == g[n_array[0]]) && 
										(color == g[n_array[1]]) &&
										(color == g[k*gsize+n]) && 
										(color == g[l*gsize+n]) &&
										(color == g[m*gsize+n]))
								{
									for(o=n+1;o<gsize-sgsize+7;o++)
									{
										if(o==i || o==j) continue;
										int o_array[2];

										if(i<o) o_array[0]=i*gsize+o;
										else	o_array[0]=o*gsize+i;

										if(j<n) o_array[1]=j*gsize+o;
										else	o_array[1]=o*gsize+j;
										if( (color == g[o_array[0]]) && 
												(color == g[o_array[1]]) &&
												(color == g[k*gsize+o]) && 
												(color == g[l*gsize+o]) &&
												(color == g[m*gsize+o]) &&
												(color == g[n*gsize+o]))
										{count++;}
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

/*
   PaleyGraph as starting point
 */
int* PaleyGraph(int gsize){
	int i, k;
	int *g = (int * )malloc(gsize * gsize * sizeof(int));
	memset(g, 0, gsize * gsize);

	for( i=0; i<gsize; i++){
		for( k= 1; k< (gsize)/ 2; k++)
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

/*
 * CopyGraph 
 *
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
	int gt[2];
	int t=INITEM;
	int sock = -1;
	FILE *fp;
	fp=fopen("graph.state", "a");

	if(argc > 2)
		sock = open_socket("169.231.19.110");
	
	/*
	 * Starting with Paley size 101
	 */
	gsize = 109;
	g = PaleyGraph(109);

	/*
	 * make a fifo to use as the taboo list
	 */
	taboo_list = FIFOInitEdge(TABOOSIZE);
	if(taboo_list == NULL) {
		exit(1);
	}

	/*
	 * start out with all zeros

	 memset(g,0,gsize*gsize*sizeof(int));
	 val = 0, iter = 0, jter=0;
	 for( iter=0; iter<gsize; iter++){
	 for( jter = 0; jter< gsize; jter++){
	 g[iter*gsize + jter]  = val;
	 val = 1 - val; 
	 }
	 }
	 PrintGraph(g, gsize);
	 */

	int best_clique = INITEM;
	int flag = 0; int min_count = BIGCOUNT;

	/*
	 * while we do not have a publishable result
	 */
	while(gsize < 200)
	{

		printf("Graph size is %d\n", gsize);
		/*
		 * find out how we are doing
		 */
		count = CliqueCount(g,gsize);
		int * cliques = (int*)malloc( (gsize)* (gsize) *sizeof(int));
		memset (cliques,INICLI,(gsize)* (gsize));
		/*
		 * if we have a counter example
		 */
		if(count == 0)
		{
			printf("Eureka!  Counter-example found!\n");
			min_count = BIGCOUNT;
			socket_upload(sock, g, gsize);	
			PrintGraphToFile(g,gsize,fp);
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
		best_clique = INICLI;		

		time_t tim;
		srand((unsigned) time(&tim));

		// Introducing randomization into system
		int choice = rand() % 10;
		printf("choice: %d \n", choice);
		if (choice == 0){
			i=rand()%gsize;
			j=rand()%gsize;
			while (FIFOFindEdge(taboo_list,i,j)){
				i=rand()%gsize;
				j=rand()%gsize;
			}
			gt[0]=i;
			gt[1]=j;
			g[gt[0]*gsize+gt[1]] = 1 - g[gt[0]*gsize+gt[1]];

			// Getting clique count of edges on flipping - assuming random move is best
			best_clique=CliqueCount_D(g,gsize,gt[0],gt[1], 0);

			g[gt[0]*gsize+gt[1]] = 1 - g[gt[0]*gsize+gt[1]];
		}
		else{

			for(i=0; i < gsize; i++)
			{
				for(j=i+1; j < gsize; j++)
				{
					if(!FIFOFindEdge(taboo_list,i,j)){
						// Clique Count without flip
						int old_clique = CliqueCount_D(g,gsize,i,j, 0);

						// Clique Count with flip
						int new_clique = CliqueCount_D(g,gsize,i,j, 1);

						// Improvement with flip
						cliques[i*gsize+j] = count + new_clique - old_clique;

					}
				}
			}

			// Finding edges in clique
			for(i=0; i < gsize; i++)
			{
				for(j=i+1; j < gsize; j++)
				{
					if(!FIFOFindEdge(taboo_list,i,j) )
					{
						if(cliques[i*gsize+j] < best_clique )
						{
							best_clique = cliques[i*gsize+j];
							gt[0] = i;
							gt[1] = j;
						}
					}
				}

			}
		}

		if(min_count > count)
		{
			min_count = count;
		}

		if (best_clique <= count){
			g[gt[0]*gsize+gt[1]] = 1 - g[gt[0]*gsize+gt[1]];

			if( CliqueCount(g, gsize) > count )
			{
				g[gt[0]*gsize+gt[1]] = 1 - g[gt[0]*gsize+gt[1]];				
			}
		}


		FIFOInsertEdge(taboo_list,gt[0],gt[1]);
		t-=DTEM;	
		if (t==0){
			t=INITEM;
		}

		printf("size: %d, temperature: %d, count: %d, best_clique: %d\n", gsize,t, count, best_clique);

		free(cliques);

	}

	FIFODeleteGraph(taboo_list);


	return(0);

}
