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

#define min(a,b)	  ((a) < (b) ? (a) : (b))
#define MAXNITEMS         1000000
#define MAXNTHREADS	  100

/**
	gcc -oprodcons6 prodcons6.c -lpthread	
*/

int 	nitems;
int 	buff[MAXNITEMS];
struct {
	pthread_mutex_t mutex ;
	int 		nput;
	int 		nval;
} put = {
	PTHREAD_MUTEX_INITIALIZER
};

struct {
	pthread_mutex_t mutex;
	pthread_cond_t  cond;
	int 		nready;
} nready = {
	PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER
};


void *produce(void *), *consume(void*);

int main(int argc, char **argv)
{
	int i, nthreads, count[MAXNTHREADS];
	pthread_t tid_produce[MAXNTHREADS], tid_consume;

	if( argc != 3 )
		fprintf(stderr, "usage:prodcons2 <#items> <#threads>");

	nitems = min(atoi(argv[1]), MAXNITEMS);
	nthreads = min(atoi(argv[2]),MAXNTHREADS);
	
//	set_concurrency(nthreads);
	
	for(i = 0; i < nthreads; i++){
		count[i] = 0;
		pthread_create(&tid_produce[i], NULL, produce, &count[i]);
	}

	pthread_create(&tid_consume, NULL, consume, NULL );


	for(i = 0 ; i< nthreads ; i++){
		pthread_join(tid_produce[i], NULL);
		printf("count[%d] = %d\n", i, count[i]);
	}

	pthread_join(tid_consume, NULL);

	exit(0);

}


void *produce(void *arg)
{
	for( ; ; ){
		pthread_mutex_lock(&put.mutex);
		if(put.nput >= nitems) {
			pthread_mutex_unlock(&put.mutex);
			return NULL;
		}

		buff[put.nput] = put.nval;
		put.nput++;
		put.nval++;
		pthread_mutex_unlock(&put.mutex);

		pthread_mutex_lock(&nready.mutex);
		if(nready.nready == 0 )
			pthread_cond_signal(&nready.cond);
		nready.nready++;
		pthread_mutex_unlock(&nready.mutex);

		*( (int *) arg) += 1 ;
	}
}


void  *consume(void *args)
{
	int i;
	
	for( i =0 ; i < nitems; i++){
		pthread_mutex_lock(&nready.mutex);
		while( nready.nready == 0 )
			pthread_cond_wait(&nready.cond, &nready.mutex);
		nready.nready--;
		pthread_mutex_unlock(&nready.mutex);

		if(buff[i] != i)
			printf("buff[%d] = %d\n",i, buff[i]);
	}
	return NULL;
}

