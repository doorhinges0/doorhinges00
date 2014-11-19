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
#include <pthread.h>
#include <sys/sem.h>

#define SVSEM_MODE (SEM_R | SEM_A | SEM_R>>3 | SEM_R>>6)
#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP| S_IROTH )
#define NBUFF  10
#define SEM_MUTEX  "mutex"
#define SEM_NEMPTY "nempty"
#define SEM_NSTORED "nstored"
#define POSIX_IPC_PREFIX "/"
#ifndef PATH_MAX
#define PATH_MAX  1024
#endif

int main(int argc ,char **argv)
{
	int c,oflag,semid,nsems;
	
	oflag = IPC_CREAT;
	//oflag = SVSEM_MODE | IPC_CREAT;
	while( ( c = getopt(argc,argv,"e")) != -1 ){
		switch(c){
		case 'e':
			oflag |= IPC_EXCL;
			break;
		}
	}

	if( optind != argc - 2 )
		fprintf(stderr,"usage: semcreate [-e] <pathname> <nsems>");
	nsems = atoi(argv[optind+1]);

	semid = semget(ftok(argv[optind],0), nsems, oflag);

	exit(0);
}


	












