#include "common.h"
#include "base_func.h"

typedef struct {
	pid_t	child_pid;
	int	child_pipefd;
	int 	child_status;
	long	child_count;
} Child;

Child *cptr;

static int nchildren;

void sig_int (int signo);
void sig_alrm(int signo);
pid_t child_make(int i, int listenfd, int addrlen);

int main(int argc, char **argv)
{
	int listenfd, i, navail, maxfd, nsel, connfd, rc;
	void	sig_int(int);
	pid_t	child_make(int, int, int);
	ssize_t	n;
	fd_set	rset, masterset;
	socklen_t	addrlen, clilen;
	struct sockaddr *cliaddr;

	if ( argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		fprintf(stderr, "usage: server_serv_sockpair [ <host> ] <port#> <#children>");

	FD_ZERO(&masterset);
	FD_SET(listenfd, &masterset);
	maxfd = listenfd;
	cliaddr = malloc(addrlen);

	nchildren = atoi(argv[argc-1]);
	navail = nchildren;
	cptr = calloc(nchildren, sizeof(Child));

	for (i = 0; i < nchildren; i++) {
		child_make(i, listenfd, addrlen);
		FD_SET(cptr[i].child_pipefd, &masterset);
		maxfd = max(maxfd, cptr[i].child_pipefd);
	}

	signal (SIGALRM, sig_alrm);
	signal (SIGINT, sig_int);

	for ( ;  ; ) {
		rset = masterset;
		if (navail <= 0)
			FD_CLR(listenfd, &rset);
		nsel = select(maxfd + 1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {
			clilen = addrlen;
			connfd = accept(listenfd, cliaddr, &clilen);

			for ( i = 0; i < nchildren; i++)
				if (cptr[i].child_status == 0)
					break;

			if (i == nchildren)
				fprintf(stderr, "no available children"), exit(1);
			cptr[i].child_status = 1;
			cptr[i].child_count++;
			navail--;

			n = write_fd(cptr[i].child_pipefd, "", 1, connfd);
			close(connfd);
	
			if (--nsel == 0)
				continue;
		}

		for (i = 0; i < nchildren; i++) {
			if (FD_ISSET(cptr[i].child_pipefd, &rset)) {
				if ( (n = read(cptr[i].child_pipefd, &rc, 1)) == 0)
					fprintf(stderr, "child %d terminated unexpectedly",i ), exit(1);
				cptr[i].child_status = 0;
				navail++;
				if (--nsel == 0)
					break;
			}
		}
	}

}

void sig_alrm(int signo)
{
	printf("sig_alrm\n");
}

void sig_int(int signo)
{
	int	i;
	void pr_cpu_time(void);
printf("sig_int==0000=\n");

	for ( i = 0; i < nchildren; i++) {
		printf("cptr[%d]=%d\n",i,cptr[i].child_pid);
		kill(cptr[i].child_pid, SIGTERM);
	}
		
printf("11111111sig_int===\n");
	while (wait(NULL)>0)
		;

printf("sig_int===\n");

	if (errno != ECHILD)
		printf("wait error"),exit(1);
//fprintf(stderr, "wait error");

	pr_cpu_time();

	for (i = 0; i < nchildren; i++)
		printf( "child %d, %ld connections\n", i, cptr[i].child_count);
//fprintf(stderr, "child %d, %ld connections\n", i, cptr[i].child_count);

	exit(0);
}

pid_t child_make(int i, int listenfd, int addrlen)
{
	int	sockfd[2];
	pid_t	pid;
	void	child_main(int, int, int);

	socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd);

//	if ( (pid == fork()) >0) {
	if ( (pid = fork()) >0) {
		close(sockfd[1]);
		cptr[i].child_pid = pid;
		cptr[i].child_pipefd = sockfd[0];
		cptr[i].child_status = 0;
		return pid;
	}

	dup2(sockfd[1], STDERR_FILENO);
	close(sockfd[0]);
	close(sockfd[1]);
	close(listenfd);
	child_main(i, listenfd, addrlen); 		

}

void child_main( int i, int listenfd, int addrlen)
{
	char	c;
	int	connfd;
	ssize_t	n;
	void	web_child(int);

	printf("==11111=child %ld starting \n",(long) getpid());
	for ( ; ; ) {
//fprintf(stderr," before read_fd ");
printf("--------------1");
		if ( (n= read_fd(STDERR_FILENO, &c, 1, &connfd)) == 0)
			fprintf(stderr, " read_fd returned 0"), exit(1);
//fprintf(stderr," after read_fd");
		if (connfd < 0)
			fprintf(stderr, "no descriptor from read_fd"), exit(1);

//fprintf(stderr," child_main next do web_child");
printf(" child_main next do web_child");
		web_child(connfd);
		close(connfd);
		
		write(STDERR_FILENO, "", 1);
	}

}





	

