#include "common.h"

struct shmstruct {
	int count;
};

/**
*
*  gcc -opxshm_server1 pxshm_server1.c -lrt	
*/

sem_t *mutex;

int main(int argc, char **argv)
{
	int fd;
	struct shmstruct *ptr;

	if( argc != 3)
		fprintf(stderr, "usage: pxshm_server1 <shmname> <semname> ");

	shm_unlink(argv[1]);
	fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
	ftruncate(fd, sizeof(struct shmstruct));
	ptr = mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	sem_unlink(argv[2]);
	mutex = sem_open(argv[2], O_RDWR | O_CREAT | O_EXCL, FILE_MODE, 1); 
	sem_close(mutex);

	exit(0);
}
