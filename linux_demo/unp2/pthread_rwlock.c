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

typedef struct{
	pthread_mutex_t rw_mutex;
	pthread_cond_t rw_condreaders;
	pthread_cond_t rw_condwriters;
	int	rw_magic;
	int 	rw_nwaitreaders;
	int	rw_nwaitwriters;
	int 	rw_refcount;
} pthread_rwlock_t;

#define RW_MAGIC	0x19283746

#define PTHREAD_RWLOCK_INITIALIZER{ PTHREAD_MUTEX_INITIALIZER, \
		PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, \
		RW_MAGIC, 0 , 0 , 0 }

typedef int pthread_rwlockattr_t;

int pthread_rwlock_destory(pthread_rwlock_t *);
int pthread_rwlock_init(pthread_rwlock_t *,pthread_rwlockattr_t *);
int pthread_rwlock_rdlock(pthread_rwlock_t *);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *);
int pthread_rwlock_trywrlock(pthread_rwlock_t *);
int pthread_rwlock_unlock(pthread_rwlock_t *);
int Pthread_rwlock_wrlock(pthread_rwlock_t *);
int Pthread_rwlock_destory(pthread_rwlock_t *);
int Pthread_rwlock_init(pthread_rwlock_t *,pthread_rwlockattr_t *);
int Pthread_rwlock_rdlock(pthread_rwlock_t *);
int Pthread_rwlock_tryrdlock(pthread_rwlock_t *);
int Pthread_rwlock_trywrlock(pthread_rwlock_t *);
int Pthread_rwlock_unlock(pthread_rwlock_t *);
int Pthread_rwlock_wrlock(pthread_rwlock_t *);

#endif

int pthread_rwlock_init(pthread_rwlock_t *rw, pthread_rwlockattr_t *attr)
{
	int result;
	
	if(attr != NULL){
		fprintf(stderr," attr cann't be NULL");
		return ;
	}
	
	if( (result = pthread_mutex_init(&rw->rw_mutex, NULL)) != 0)
		goto err1;
	if( (result = pthread_cond_init(&rw->rw_condreaders, NULL)) != 0 )
		goto err2;
	if( (result = pthread_cond_init(&rw->rw_condwriters, NULL)) != 0)
		goto err3;

	rw->rw_nwaitreaders = 0;
	rw->rw_nwaitwriters =0;
	rw->rw_redcount = 0;
	rw->rw_magic = RW_MAGIC;
	
	return 0;
	
	err3:
		pthread_cond_destory(&rw->rw_condreaders);
	err2:
		pthread_cond_destory(&rw->rw_mutex);
	err1:
		return result;
}

int pthread_rwlock_destory(pthread_rwlock_t *rw)
{
	if(rw->rw_magic != RW_MAGIC)
		return EINVAL;
	if(rw->rw_refcount != 0 || rw->rw_nwaitreaders != 0 || rw->rw_nwaitwriters !=  0)
		return EBUSY;

	pthread_mutex_destroy(&rw->rw_mutex);
	pthread_cond_destroy(&rw->rw_condreaders);
	pthread_cond_destroy(&rw->rw_condwriters);
	rw->rw_magic = 0;

	return 0;
}


int pthread_rwlock_rdlock(pthread_rwlock_t *rw)
{
	int result;

	if(rw->rw_magic != RW_MAGIC)
		return EINVAL;

	if( (result = pthread_mutex_lock(&rw->rw_mutex)) != 0)
		return result;

	while(rw->rw_refcount < 0 || rw->rw_nwaitwriters > 0){
		rw->rw_nwaitreaders++;
		pthread_cleanup_push(rwlock_cancelrdwait, (void*)rw);
		result = pthread_cond_wait(&rw->rw_condreaders, &rw->rw_mutex);
		pthread_cleanup_pop(0);
		rw->rw_nwaitreaders--;
		if(result != 0)
			break;
	}

	if(result == 0)
		rw->rw_refcount++;

	pthread_mutex_unlock(&rw->rw_mutex);
	return result;
}


int pthread_rwlock_tryrdlock(pthread_rwlock_t *rw)
{
	int result;

	if(rw->rw_magic != RW_MAGIC)
		return EINVAL;

	if( (result = pthread_mutex_lock(&rw->rw_mutex)) != 0 )
		return result;

	if(rw->rw_refcount < 0 || rw->rw_nwaitwriters > 0)
		result = EBUSY;
	elst
		rw->rw_refcount++;

	pthread_mutex_unlock(&rw->rw_mutex);
	return result;
}


int pthread_rwlock_wrlock(pthread_rwlock_t *rw)
{
	int result;

	if(rw->rw_magic != MAGIC)
		return EINVAL;

	if( (result = pthread_mutex_lock(&rw->rw_mutex)) != 0 )
		return result;

	while(rw->rw_refcount != 0){
		rw->rw_nwaitwriters++;
		pthread_cleanup_push(rwlock_cancelwrwait,(void*)rw);
		result = pthread_cond_wait(&rw->rw_condwriters, &rw->rw_mutex);
		pthread_cleanup_pop(0);
		rw->rw_nwaitwriters--;
		if(result != 0 )
			break;
	}

	if( result == 0 )
		rw->rw_ref_count = -1;
	
	pthread_mutex_unlock(&rw->rw_mutex);
	return result;
}


int pthread_rwlock_trywrlock(pthread_rwlock_t *rw)
{
	int result;
	
	if(rw->rw_magic != RW_MAGIC)
		return EINVAL;

	if( ( result = pthread_mutex_lock(&rw->rw_mutex)) != 0 )
		return result;

	if(rw->rw_refcount != 0 )
		result = EBUSY;
	else
		rw->rw_refcount = -1 ;

	pthread_mutex_unlock(&rw->rw_mutex);
	return result;
}


int pthread_rwlock_unlock(pthread_rwlock_t *rw)
{
	int result;

	if(rw->rw_magic != RW_MAGIC)
		return EINVAL;

	if( (result = pthread_mutex_lock(&rw->rw_mutex)) != 0)
		return result;

	if(rw->rw_refcount > 0)
		rw->rw_refcount--;
	else if(rw->rw_refcount == -1)
		rw->rw_refcount =0;
	else
		fprintf(stderr,"rw_refcount=%d",rw->rw_redcount);

	if(rw->rw_nwaitwriters >0 ){
		if(rw->rw_refcount == 0 )
			result = pthread_cond_signal(&rw->rw_condwriters);
	} else if(rw->rw_nwaitreaders > 0 )
		result = pthread_cond_broadcast(&rw->rw_condreaders);

	pthread_mutex_unlock(&rw->rw_mutex);
	return result;
}

/**
*
*	main 
*	gcc -opthread_rwlock pthread_rwlock.c -lpthread	
*
*/


pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_t tid1, tid2;
void *thread1(void*),*thread2(void*);

int main(int argc, int **argv)
{
	void *status;

	pthread_create(&tid1, NULL, thread1, NULL);
	sleep(1);
	pthread_create(&tid2, NULL, thread2, NULL);
	
	pthread_join(tid2,&status);
	if(status != PTHREAD_CANCELED)
		printf("thread2 status =%p\n",status);
	pthread_join(tid1,&status);
	if(status != NULL)
		printf("thread1 status =%p\n",status);

	printf("rw_refcount =%d,rw_nwaitreaders=%d,rw_nwaitwriters=%d\n",rwlock.rw_refcount,rwlock.rw_nwaitreaders,rwlock.rw_nwaitwriters);

	pthread_rwlock_destroy(&rwlock);
	exit(0);
}


void *thread1(void *arg)
{
	pthread_rwlock_rdlock(&rwlock);
	printf("thread1 got a read lock\n");
	sleep(3);
	pthread_cancel(tid2);
	sleep(3);
	pthread_rwlock_unlock(&rwlock);
	
	return NULL;
}
	

void *thread2(void *arg)
{
	printf("thread2 trying to obtain a write lock\n");
	pthread_rwlock_wrlock(&rwlock);
	printf("thread2 got a write lock\n);
	sleep(1);
	pthread_rwlock_unlock(&rwlock);
	return NULL;
}

static void rwlock_cancelrdwait(void *arg)
{
	pthread_rwlock_t *rw;

	rw = arg;
	rw->rw_nwaitreaders--;
	pthread_mutex_unlock(&rw->rw_mutex);
}

static void rwlock_cancelwrwait(void *arg)
{

	pthread_rwlock_t *rw;
	
	rw = arg ;
	rw->rw_nwaitwriters--;
	pthread_mutex_unlock(&rw->rw_mutex);
}




