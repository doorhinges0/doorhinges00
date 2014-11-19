#include "common.h"
#include "base_func.h"

static struct flock lock_it, unlock_it;
static int lock_fd = -1;

void my_lock_init(char *pathname)
{
	char	lock_file[1024];

	strncpy(lock_file, pathname, sizeof(lock_file));
	lock_fd = mkstemp(lock_file);

	if (lock_fd == -1)
		fprintf(stderr, "mkstemp failed"),exit(1);
	
	unlink(lock_file);

	lock_it.l_type = F_WRLCK;
	lock_it.l_whence = SEEK_SET;
	lock_it.l_start = 0;
	lock_it.l_len = 0;

	unlock_it.l_type = F_UNLCK;
	unlock_it.l_whence = SEEK_SET;
	unlock_it.l_start = 0;
	unlock_it.l_len = 0;
}


void my_lock_wait()
{
	int	rc;
	
	while ( (rc = fcntl(lock_fd, F_SETLKW, &lock_it)) < 0) {
		if (errno == EINTR)
			continue;
		else
			fprintf(stderr, "fcntl error for my_lock_wait"), exit(1);
	}
}


void my_lock_release()
{
	if (fcntl(lock_fd, F_SETLKW, &unlock_it) < 0)
		fprintf(stderr, "fcntl error for my_lock_release"), exit(1);
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





	

