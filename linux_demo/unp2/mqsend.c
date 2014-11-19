#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <mqueue.h>

#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP| S_IROTH )
#define MAXLINE 4096 


int main(int argc , char **argv)
{
	mqd_t mqd;
	void *ptr;
	size_t len;
	uint32_t prio;
	int flags;

	flags = O_RDWR | O_CREAT;
	
	if( argc != 4 )
		fprintf(stderr,"usage :mqsend <name> <#byte> <priority> ");
	
	len = atoi(argv[2]);
	prio = atoi(argv[3]);

	mqd = mq_open(argv[1], flags ,FILE_MODE , NULL );
	
	if( mqd == -1 )
		fprintf(stderr," creat mq error");
	else
		fprintf(stdout,"\nmqd:%d\n",mqd);

	fprintf(stdout, "calloc before\n");
	
	ptr = (char *)calloc(len, sizeof(char));
	if(ptr == NULL ){
		perror("calloc error");
	}

	fprintf(stdout, "calloc after\n");
	mq_send(mqd, ptr, len, prio);
	
	free(ptr);
	exit(0);
}


	





