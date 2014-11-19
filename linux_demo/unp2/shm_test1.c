#include "common.h"

/**
*
*  gcc -oshm_test1 shm_test1.c -lpthread	
*/

int main(int argc , char ** argv)
{
	int fd, i;
	char *ptr;
	size_t filesize, mmapsize, pagesize;

	if(argc != 4)
		fprintf(stderr, "usage: shm_test1 <pathname> <filesize> <mmapsize>");
	filesize = atoi(argv[2]);
	mmapsize = atoi(argv[3]);

	fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE);
	lseek(fd, filesize-1, SEEK_SET);
	write(fd, "", 1);

	ptr = mmap(NULL, mmapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	pagesize = sysconf(_SC_PAGESIZE);
	printf("pagesize =%ld\n",(long)pagesize);

	for( i =0 ; i< max(filesize,mmapsize); i+=pagesize){
		printf("ptr[%d] =%d\n",i, ptr[i]);
		ptr[i] = 1;
		printf("ptr[%d] = %d \n", i + pagesize-1 , ptr[i + pagesize-1]);
		ptr[i + pagesize -1] = 1;
	}
	printf("ptr[%d] = %d\n",i , ptr[i]);

	exit(0);
}

	
	

