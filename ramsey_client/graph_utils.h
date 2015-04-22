#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <ctype.h>
#include <stddef.h>
#include "jval.h"
#include "jrb.h"
#include "fifo.h"
#include "dllist.h"
#include <errno.h>


#ifndef GRAPH_UTILS_H
#define GRAPH_UTILS_H

void PrintGraph(int *g, int gsize);
int ReadGraph(char *fname, int **g, int *gsize);
int *RandomGraphGenerator(int n);
int CliqueCount(int *g,int gsize);
int CliqueCount_D(int *g, int gsize, int i, int j, int flip);

#endif
