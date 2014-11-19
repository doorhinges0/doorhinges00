#include "common.h"

struct shmstruct {
	int count;
};
sem_t *mutex;

/**
*
*  gcc -opxshm_client1 pxshm_client1.c -lrt	
*/

int main(int argc, char **argv)
{
	int fd, i, nloop;
	pid_t pid;
	struct shmstruct *ptr;

	if( argc != 4 )
		fprintf(stderr, "usage: pxshm_client <shmname> <semname> <#loop>");
	nloop = atoi(argv[3]);

	fd = shm_open(argv[1], O_RDWR, FILE_MODE);
	ptr = mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	mutex = sem_open(argv[2], 0);
	
	pid = getpid();
	for( i =0 ; i < nloop ; i++){
		sem_wait(mutex);
		printf("pid %ld: %d\n",(long)pid, ptr->count++);
		sem_post(mutex);
	}
	exit(0);
}


	

