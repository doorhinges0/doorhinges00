#include "common.h"

#define SEM_NAME "mysem"

/**
*
*  gcc -oincr2 incr2.c -lpthread	
*/
int main(int argc, char **argv)
{
	int fd, i , nloop, zero = 0;
	int *ptr;
	sem_t *mutex;

	if(argc != 3 )
		fprintf(stderr, "usage: incr2 <pathname> <#loops>\n");
	nloop = atoi(argv[2]);

	fd = open(argv[1], O_RDWR | O_CREAT , FILE_MODE);
	write(fd, &zero, sizeof(int));
	ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );
	close(fd);
	
	mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, FILE_MODE, 1 );
	sem_unlink(SEM_NAME);

	setbuf(stdout,NULL);
	if(fork() == 0 ){
		for ( i = 0 ; i < nloop ; i++){
			sem_wait(mutex);
			printf("child:%d\n",(*ptr)++);
			sem_post(mutex);
		}
		exit(0);
	}

	for( i = 0 ; i < nloop ; i++){
		sem_wait(mutex);
		printf("parent:%d \n",(*ptr)++);
		sem_post(mutex);
	}
	exit(0);
}




