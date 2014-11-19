#include "common.h"
#include "base_func.h"
#define MAXNCLI	32

typedef struct {
	pthread_t thread_tid;
	long long	thread_count;
} Thread;
Thread *tptr;


int	clifd[MAXNCLI], iget, iput;


pthread_mutex_t clifd_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clifd_cond = PTHREAD_COND_INITIALIZER;
static int nthreads;

#if 0
void sig_int(int);
void thread_make(int);
#endif 
int main(int argc, char **argv)
{
	int  i, listenfd, connfd;
#if 1
	void sig_int(int);
	void thread_make(int);
#endif
	socklen_t addrlen, clilen;
	struct sockaddr *cliaddr;

	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4 )
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		fprintf(stderr, "usage: server_serv_thread2 [ <host> ] <port#> <#children>");
	cliaddr = malloc(addrlen);	

	nthreads = atoi(argv[argc - 1]);
	tptr = calloc(nthreads, sizeof(Thread));
	iget = iput = 0;
	
	for (i = 0; i < nthreads; i++)
		thread_make(i);

	signal(SIGINT, sig_int);

	for (; ; ) {
		clilen = addrlen;
		connfd = accept(listenfd, cliaddr, &clilen);
		
		pthread_mutex_lock(&clifd_mutex);
		clifd[iput] = connfd;
		if (++iput == MAXNCLI)
			iput = 0;
		if (iput == iget)
			fprintf(stderr, "iput = iget = %d",iput), exit(1);

		pthread_cond_signal(&clifd_cond);
		pthread_mutex_unlock(&clifd_mutex);
	}	

}

void sig_int (int signo)
{
	int	i;
	void pr_cpu_time(void);

	pr_cpu_time();

	for (i = 0; i < nthreads; i++)
		printf("thread %d, %lld connections\n", i, tptr[i].thread_count);

	exit(0);
}

void thread_make(int i)
{
	void thread_main(void *);

	pthread_create(&tptr[i].thread_tid, NULL, &thread_main, (void *) i);
	return;
}

void thread_main( void *arg)
{
	int	connfd;
	void	web_child(int);

	printf("thread %d starting\n", (int)arg);
	for ( ; ; ) {
		pthread_mutex_lock(&clifd_mutex);	

		while (iget == iput)
			pthread_cond_wait(&clifd_cond, &clifd_mutex);
		connfd = clifd[iget];
		if (++iget == MAXNCLI)
			iget = 0;

		pthread_mutex_unlock(&clifd_mutex);
		tptr[(int) arg].thread_count++;
		
		web_child(connfd);
		close(connfd);
	}
}





	

