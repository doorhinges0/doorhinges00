#include "common.h"
#include "base_func.h"

typedef struct {
	pthread_t thread_tid;
	long	thread_count;
} Thread;
Thread *tptr;

int listenfd, nthreads;
socklen_t addrlen;

pthread_mutex_t mlock1 = PTHREAD_MUTEX_INITIALIZER;


void sig_int (int signo);
pid_t child_make(int i, int listenfd, int addrlen);

int main(int argc, char **argv)
{
	int  i;
	void sig_int(int);
	void thread_make(int);

	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4 )
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		fprintf(stderr, "usage: server_serv_thread [ <host> ] <port#> <#children>");
	
	nthreads = atoi(argv[argc - 1]);
	tptr = calloc(nthreads, sizeof(Thread));	
	for (i = 0; i < nthreads; i++)
		thread_make(i);

	signal(SIGINT, sig_int);

	for (; ; )
		pause();

}

void sig_int (int signo)
{
	int	i;
	void pr_cpu_time(void);

	pr_cpu_time();

	for (i = 0; i < nthreads; i++)
		printf("thread %d, %ld connections\n", i, tptr[i].thread_count);

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
	socklen_t clilen;
	struct sockaddr *cliaddr;

	cliaddr = malloc(addrlen);

	printf("thread %d starting\n", (int)arg);
	for ( ; ; ) {
		clilen = addrlen;

		pthread_mutex_lock(&mlock1);	

		connfd = accept( listenfd, cliaddr, &clilen);

		pthread_mutex_unlock(&mlock1);
		tptr[(int) arg].thread_count++;
		
		web_child(connfd);
		close(connfd);
	}
}





	

