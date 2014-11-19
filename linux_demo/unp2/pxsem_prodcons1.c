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
#define NBUFF  10
#define SEM_MUTEX  "mutex"
#define SEM_NEMPTY "nempty"
#define SEM_NSTORED "nstored"
#define POSIX_IPC_PREFIX "/"
#ifndef PATH_MAX
#define PATH_MAX  1024
#endif

int nitems;
struct {
 int buff[NBUFF];
 sem_t *mutex, *nempty, *nstored;
} shared;

void *produce(void *), *consume(void *);
char *px_ipc_name(const char *name);


/**
*
*  gcc -opxsem_prodcons1 pxsem_prodcons1.c -lpthread	
*/

int main(int argc, char **argv)
{
	pthread_t tid_produce, tid_consume;

	if(argc != 2)
		fprintf(stderr, "pxsem_prodcons1 <#items>");
	nitems = atoi(argv[1]);
	
	shared.mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, FILE_MODE, 1);
	shared.nempty = sem_open(SEM_NEMPTY, O_CREAT | O_EXCL, FILE_MODE, 1);
	shared.nstored = sem_open(SEM_NSTORED, O_CREAT | O_EXCL, FILE_MODE, 1);

#if 0
	shared.mutex = sem_open(px_ipc_name(SEM_MUTEX), O_CREAT | O_EXCL, FILE_MODE, 1);
	shared.nempty = sem_open(px_ipc_name(SEM_NEMPTY), O_CREAT | O_EXCL, FILE_MODE, 1);
	shared.nstored = sem_open(px_ipc_name(SEM_NSTORED), O_CREAT | O_EXCL, FILE_MODE, 1);
#endif

	pthread_create(&tid_produce, NULL,produce,NULL);
	pthread_create(&tid_consume,NULL,consume,NULL);

	pthread_join(tid_produce, NULL);
	pthread_join(tid_consume, NULL);

#if 1
	sem_unlink(SEM_MUTEX);
	sem_unlink(SEM_NEMPTY);
	sem_unlink(SEM_NSTORED);
#endif 
	

#if 0
	sem_unlink(px_ipc_name(SEM_MUTEX));
	sem_unlink(px_ipc_name(SEM_NEMPTY));
	sem_unlink(px_ipc_name(SEM_NSTORED));
#endif 

	exit(0);
}


void *produce(void *arg)
{
	int i;
	
	for(i =0 ;i < nitems; i++ ){
		sem_wait(shared.nempty);
		sem_wait(shared.mutex);
		printf("put %4d\n",i);
		shared.buff[i % NBUFF] = i;
		sem_post(shared.mutex);
		sem_post(shared.nstored);
	}
	return NULL;
}


void *consume(void *arg)
{
	int i;

	for( i = 0;i < nitems; i++){
		sem_wait(shared.nstored);
		printf("get nstored\n");
		sem_wait(shared.mutex);
		if(shared.buff[i % NBUFF] != i )
			fprintf(stderr," buff[%d] = %d\n",i,shared.buff[i%NBUFF]);
		sem_post(shared.mutex);
		sem_post(shared.nempty);
	}
	return NULL;
}		

char *px_ipc_name_core(const char *name)
{
	char *dir,*dst,*slash;
	
	if((dst = malloc(PATH_MAX)) != NULL)
		return NULL;
         
	if( (dir = getenv("PX_IPC_NAME")) == NULL ) {
#ifdef POSIX_IPC_PREFIX
	dir = POSIX_IPC_PREFIX;
#else
	dir = "/tmp/";
#endif
	}

	slash = (dir[strlen(dir) - 1] == '/') ? "" :"/";
	snprintf(dst,  PATH_MAX, "%s%s%s", dir, slash, name);

	return dst;
}

char *px_ipc_name(const char *name)
{
	char *ptr;

	if( (ptr= px_ipc_name_core(name)) == NULL)
		fprintf(stderr,"px_ipc_name error for %s ",name);

	return ptr;
}


	



