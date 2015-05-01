#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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




int main(){
    int* graph = PaleyGraph(137);
    PrintGraph(graph, 137);

    return 0;
}
