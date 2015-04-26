#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>

int open_socket();
void socket_upload(int sock, int *g,int gsize);
