#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


#define MAXSIZE (541)
#define sgsize (7)
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
