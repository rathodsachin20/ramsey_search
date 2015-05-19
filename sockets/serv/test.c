#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

//return file name which has best best count
char *getbcfile(const char *dirname){
	DIR *dir;
	struct dirent *ent;
	size_t nbytes=256;
	char *line = (char *) malloc(nbytes+1);
	char *bcfile=(char *) malloc(nbytes+1);
	int best= INT_MAX,bc;
	ssize_t read;

	if ((dir = opendir (dirname)) != NULL) {
  		/* print all the files and directories within directory */
  		while ((ent = readdir (dir)) != NULL) {
    		//printf ("%s\n", ent->d_name);
    		if(!(strcmp(ent->d_name,"."))){
    			//printf("ignored root: %s\n",ent->d_name);
    			continue;
    		}
    		if(!(strcmp(ent->d_name,".."))){
    			//printf("ignored root: %s\n",ent->d_name);
    			continue;
    		}
    		
    		char fname[32]="";
    		strcat(fname,dirname);
    		strcat(fname,ent->d_name);
    		//printf("%s\n",fname);
    		
    		FILE* fp;
  			fp = fopen(fname,"r");
  			if((read = getline(&line, &nbytes, fp)) != -1){
         		//printf("Retrieved line of length: %zu\n", read);
          		//printf("%s", line);
          		sscanf(line, "%d", &bc);
          		if(bc<best){
          			//printf("%d",bc);
          			best = bc;
          			strcpy(bcfile,fname);
          		}
        	}
  			fclose(fp);
  		}
  		closedir (dir);
	}

	else {
  		/* could not open directory */
  		perror ("Error");
	}
	printf("bc is: %d\n",best);
	return bcfile;
}

int main(){
	const char *dirname = "./gfromc/";
	char *fname;
	fname = getbcfile(dirname);
	printf("best count is in: %s",fname);
}