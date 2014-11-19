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
	
	int c, flags;
	mqd_t mqd;
	ssize_t n;
	uint32_t prio;
	void *buff;
	struct mq_attr attr;

	flags = O_RDONLY ;
	while( ( c=getopt(argc, argv, "n")) != -1){
		switch(c){
		case 'n':
			flags |= O_NONBLOCK;
			break;
		}
	}

	if( optind != argc - 1)
		fprintf(stderr, "usage :mqreceive [-n] <name> ");

	mqd = mq_open(argv[optind], flags);
	mq_getattr(mqd, &attr);
	
	buff = malloc(attr.mq_msgsize);

	n = mq_receive(mqd, buff, attr.mq_msgsize,&prio);
	printf("read %ld bytes, priority = %u\n",(long) n, prio);

	return 0;
}


	





