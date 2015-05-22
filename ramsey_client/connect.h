#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>

int open_socket(const char* address);
void socket_upload(int sock, int *g,int gsize);
void socket_upload_2(int sock, int x, int y, int count, int gsize, int *g);
void* wait_for_server(void *arg);
void create_fifo_thread(int *sockPtr);
void PrintGraphToFile(int *g, int gsize, FILE * fout);