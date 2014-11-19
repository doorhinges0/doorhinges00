#include "common.h"

int count = 0 ;

/**
*
*  gcc -oincr1 incr1.c -lpthread	
*/
int main(int argc, char **argv)
{
	int  i,nloop ;
	sem_t *mutex;

	if( argc != 2 )
		fprintf(stderr,"usage:incr1 <#loops>");
	nloop = atoi(argv[1]);

	mutex = sem_open(SEM_NAME , O_CREAT | O_EXCL, FILE_MODE, 1);
	sem_unlink(SEM_NAME);

	setbuf(stdout, NULL);
	if( fork() == 0 ){
		for( i = 0 ; i < nloop ; i++){
			sem_wait(mutex);
			printf("child :%d\n",count++);
			sem_post(mutex);
		}
		exit(0);
	}

	for( i = 0 ; i < nloop ; i++){
		sem_wait(mutex);
		printf("parent :%d\n",count++);
		sem_post(mutex);
	}
	exit(0);
}


