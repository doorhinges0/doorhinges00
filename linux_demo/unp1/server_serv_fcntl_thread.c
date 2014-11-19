#include "common.h"
#include "base_func.h"

static pthread_mutex_t *mptr;

void my_lock_init(char *pathname)
{
	int	fd;
	pthread_mutexattr_t mattr;

	fd = open("/dev/zero", O_RDWR, 0);

	mptr = mmap(0, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (mptr == MAP_FAILED)
		fprintf(stderr, "mmap error"), exit(1);		

	close(fd);

	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mptr, &mattr);	

}


void my_lock_wait()
{
	pthread_mutex_lock(mptr);
}


void my_lock_release()
{
	pthread_mutex_unlock(mptr);
}


static int nchildren;
static pid_t *pids;

void sig_int (int signo);
pid_t child_make(int i, int listenfd, int addrlen);

int main(int argc, char **argv)
{
	int listenfd, i;
	socklen_t addrlen;
	void sig_int(int);
	pid_t child_make(int, int , int);

	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4 )
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		fprintf(stderr, "usage: server_serv02 [ <host> ] <port#> <#children>");
	
	nchildren = atoi(argv[argc -1 ]);
	pids = calloc(nchildren, sizeof(pid_t));

	my_lock_init("tmp_XXXXXX");	

	for (i = 0; i < nchildren; i++)
		pids[i] = child_make( i,listenfd, addrlen);

	signal(SIGINT, sig_int);

	for ( ; ; )
		pause();
}

void sig_int (int signo)
{
	int	i;
	void pr_cpu_time(void);

	for ( i = 0; i < nchildren; i++)
		kill(pids[i], SIGTERM);
	while (wait(NULL)>0)
		;

	if (errno != ECHILD)
		fprintf(stderr, "wait error");

	pr_cpu_time();
	exit(0);
}

pid_t child_make(int i, int listenfd, int addrlen)
{
	pid_t pid;
	void child_main(int ,int, int);

	if ( (pid = fork()) > 0 )
		return pid;

	child_main(i, listenfd, addrlen);
}

void child_main( int i, int listenfd, int addrlen)
{
	int	connfd;
	void	web_child(int);
	socklen_t clilen;
	struct sockaddr *cliaddr;

	cliaddr = malloc(addrlen);

	printf("child %ld starting\n", (long)getpid());
	for ( ; ; ) {
		clilen = addrlen;

		my_lock_wait();		

		connfd = accept( listenfd, cliaddr, &clilen);

		my_lock_release();
		
		web_child(connfd);
		close(connfd);
	}
}





	

