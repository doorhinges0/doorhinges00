#include "common.h"

void str_cli(FILE *fp, int sockfd);

ssize_t readline(int fd, void *vptr, size_t maxlen);

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2)
		fprintf(stderr, "usage: tcpcliserv_tcpcli01 <Ipaddress>");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (SA*)&servaddr, sizeof(servaddr));

	str_cli(stdin,sockfd);

	exit(0);
}

void str_cli(FILE *fp, int sockfd)
{
	pid_t pid;
	char sendline[MAXLINE], recvline[MAXLINE];

	if ( (pid = fork()) == 0){
		while ( readline(sockfd, recvline, MAXLINE) > 0 )
			fputs(recvline, stdout);
		kill(getppid(), SIGTERM);
		exit(0);
	}

	while ( fgets(sendline, MAXLINE, fp) != NULL)
		write(sockfd, sendline, strlen(sendline));

	shutdown(sockfd, SHUT_WR);
	pause();
	return ;

}

#if 0
void str_cli(FILE *fp, int sockfd)
{
	int maxfdp1, stdineof;
	fd_set	rset;
	char	buf[MAXLINE];
	int 	n;

	stdineof = 0;
	FD_ZERO(&rset);

	for( ; ; ){
		if (stdineof == 0 )
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		select(maxfdp1,&rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)){
			if ( (n = read(sockfd, buf, MAXLINE)) == 0 ){
				if(stdineof == 1)
					return;
				else
					fprintf(stderr, "strcli: server terminated prematurely"),exit(1);
			}
			write(fileno(stdout),buf, n);
		}

		if ( FD_ISSET(fileno(fp), &rset)) {
			if ( (n= read(fileno(fp), buf, MAXLINE)) == 0 ){
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			write(sockfd, buf, n);
		}
	}
}
#endif 
		
