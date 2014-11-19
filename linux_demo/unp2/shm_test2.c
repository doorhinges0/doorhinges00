#include "common.h"

#define FILE "TEST.DATA"
#define SIZE 32768

/**
*
*  gcc -oshm_test2 shm_test2.c -lpthread	
*/

int main(int argc, char **argv)
{
	int fd, i;
	char *ptr;

	fd = open(FILE, O_RDWR | O_CREAT | O_TRUNC, FILE_MODE);
	ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	for( i = 4096 ; i <= SIZE; i += 4096){
		printf("setting file size to %d\n",i);
		ftruncate(fd, i);
		printf("ptr[%d] = %d\n", i - 1, ptr[i-1]);
	}

	exit(0);
}


