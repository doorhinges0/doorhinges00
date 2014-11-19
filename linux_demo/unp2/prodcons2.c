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
	gcc -oprodcons2 prodcons2.c -lpthread	
*/

int 	nitems;
struct {
	pthread_mutex_t mutex ;
	int		buff[MAXNITEMS];
	int 		nput;
	int 		nval;
} shared = {
	PTHREAD_MUTEX_INITIALIZER
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

	for(i = 0 ; i< nthreads ; i++){
		pthread_join(tid_produce[i], NULL);
		printf("count[%d] = %d\n", i, count[i]);
	}

	pthread_create(&tid_consume, NULL, consume, NULL);
	pthread_join(tid_consume, NULL);

	exit(0);

}


void *produce(void *arg)
{
	for( ; ; ){
		pthread_mutex_lock(&shared.mutex);
		if(shared.nput >= nitems){
			pthread_mutex_unlock(&shared.mutex);
			return NULL;
		}

		shared.buff[shared.nput] = shared.nval;
		shared.nput++;
		shared.nval++;
		pthread_mutex_unlock(&shared.mutex);
		*((int *) arg) += 1;
	}
}


void  *consume(void *args)
{
	int i;
	
	for( i =0 ; i < nitems; i++){
		if(shared.buff[i] != i )
			printf("buff[%d] = %d\n",i, shared.buff[i]);
	}

	return NULL;
}

