#include "common.h"

struct shared{
	sem_t mutex;
	int count;
} shared;


/**
*
*  gcc -oincr3 incr3.c -lpthread	
*/

int main(int argc, char **argv)
{
	int fd, i, nloop;
	struct shared *ptr;

	if(argc != 3)
		fprintf(stderr, "usage :incr3 <pathname> <#loops>");
	nloop = atoi(argv[2]);

	fd = open(argv[1], O_RDWR | O_CREAT , FILE_MODE );
	write(fd,&shared,sizeof(struct shared));
	ptr = mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	sem_init(&ptr->mutex, 1 , 1);
	setbuf(stdout, NULL);

	if(fork() == 0){
		for( i = 0; i < nloop; i++){
			sem_wait(&ptr->mutex);
			printf("child :%d\n", ptr->count++);
			sem_post(&ptr->mutex);
		}
		exit(0);
	}

	for( i = 0 ; i < nloop ; i++){
		sem_wait(&ptr->mutex);
		printf("parent :%d\n",ptr->count++);
		sem_post(&ptr->mutex);
	}
	exit(0);
}

	
