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
#include <semaphore.h>

#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP| S_IROTH )
#define NBUFF  3
#define BUFFSIZE 1024
#define SEM_MUTEX  "mutex"
#define SEM_NEMPTY "nempty"
#define SEM_NSTORED "nstored"
#define POSIX_IPC_PREFIX "/"
#ifndef PATH_MAX
#define PATH_MAX  1024
#endif

struct {
	struct {
		char data[BUFFSIZE];
		ssize_t n;
	} buff[NBUFF];
	sem_t mutex, nempty, nstored;
} shared;


int fd;
void *produce(void *), *consume(void *);
char *px_ipc_name(const char *name);


/**
*
*  gcc -opxsem_mycat2 pxsem_mycat2.c -lpthread	
*/

int main(int argc, char **argv)
{
	pthread_t tid_produce, tid_consume;

	if(argc != 2)
		fprintf(stderr,"usage: pxsem_mycat2 <pathname>");

	fd = open(argv[1], O_RDONLY);

	sem_init(&shared.mutex, 0 , 1);
	sem_init(&shared.nempty, 0, NBUFF);
	sem_init(&shared.nstored, 0, 0);
	
	pthread_create(&tid_produce, NULL, produce, NULL);
	pthread_create(&tid_consume, NULL, consume, NULL);

	fprintf(stderr, "create threads ok !\n");

	pthread_join(tid_produce,NULL);
	pthread_join(tid_consume,NULL);

	sem_destroy(&shared.mutex);
	sem_destroy(&shared.nempty);
	sem_destroy(&shared.nstored);

	exit(0);
}


void *produce(void *arg)
{
	int i;
	
	for( i = 0 ; ; ){
		sem_wait(&shared.nempty);
		fprintf(stderr," get nempty\n");
		sem_wait(&shared.mutex);
		/* critical region */
		sem_post(&shared.mutex);

		shared.buff[i].n = read(fd,shared.buff[i].data, BUFFSIZE);
		if(shared.buff[i].n == 0 ){
			sem_post(&shared.nstored);
			return NULL;
		}
		if( ++i>= NBUFF)
			i = 0 ;

		sem_post(&shared.nstored);
	}
}


void *consume(void *arg)
{
	int i;

	for( i = 0 ; ; ){
		sem_wait(&shared.nstored);
		fprintf(stderr, " get nstored\n");
		sem_wait(&shared.mutex);
		/* critical region */	
		sem_post(&shared.mutex);

		if(shared.buff[i].n == 0 )
			return NULL;
		write(STDOUT_FILENO, shared.buff[i].data, shared.buff[i].n);
		if( ++i >= NBUFF)
			i=0;

		sem_post(&shared.nempty);
	}
}
		


