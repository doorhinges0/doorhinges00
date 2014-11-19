#include "common.h"
#define MESGSIZE	256
#define NMESG		16

struct shmstruct {
	sem_t mutex;
	sem_t nempty;
	sem_t nstored;
	int   nput;
	long  noverflow;
	sem_t noverflowmutex;
	long  msgoff[NMESG];
	char  msgdata[NMESG * MESGSIZE];
};

/**
*
*  gcc -opxshm_client2 pxshm_client2.c -lrt
*  ./pxshm_client2 serv2 50 0 	
*/

int main(int argc, char **argv)
{
	int fd, i, nloop, nusec;
	pid_t pid;
	char mesg[MESGSIZE];
	long offset;
	struct shmstruct *ptr;

	if( argc!= 4)
		fprintf(stderr, "usage: pxshm_client2 <name> <#loops> <#usec>");
	nloop = atoi(argv[2]);
	nusec = atoi(argv[3]);

	fd = shm_open(argv[1], O_RDWR, FILE_MODE);
	ptr = mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	pid = getpid();
	for( i = 0 ; i < nloop ; i++){
		usleep(nusec);
		snprintf(mesg, MESGSIZE, "pid %ld: message %d", (long)pid, i);
fprintf(stderr, "mesg:%s\n",mesg);		
		if(sem_trywait(&ptr->nempty) == -1 ){
			if( errno == EAGAIN){
				sem_wait(&ptr->noverflowmutex);
				ptr->noverflow++;
				sem_post(&ptr->noverflowmutex);
				continue;
			}else
				fprintf(stderr, "sem_trywait error");
		}
		
		sem_wait(&ptr->mutex);
		offset = ptr->msgoff[ptr->nput];
		if( ++(ptr->nput) >= NMESG)
			ptr->nput = 0;
		sem_post(&ptr->mutex);
		strcpy(&ptr->msgdata[offset], mesg);
fprintf(stderr, "====:%s\n",mesg);	
		sem_post(&ptr->nstored);
	}
	exit(0);
}
	
	
