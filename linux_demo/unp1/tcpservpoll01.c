
/*
 * tcpservpoll01.c
 *
 *  Created on: 2014-1-12
 *      Author: Administrator
 */

#include "common.h"

int main(int argc, char **argv)
{
	int i, maxi, listenfd, connfd, sockfd;
	int nready;
	ssize_t n;
	char buf[MAXLINE];
	socklen_t clilen;
	struct pollfd client[OPEN_MAX];
	struct sockaddr_in cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (SA*)&servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	client[0].fd = listenfd;
//	client[0].events = POLLRDNORM;
	client[0].events = POLLIN;
	for ( i = 1; i < OPEN_MAX; i++)
		client[i].fd = -1;
	maxi = 0 ;

	for( ; ; ){
		nready = poll(client, maxi + 1, INFTIM);

		if (client[0].revents & POLLRDNORM){
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (SA*)&cliaddr, &clilen);

			for ( i= 1; i < OPEN_MAX; i++)
				if (client[i].fd < 0){
					client[i].fd = connfd;
					break;
				}
			if ( i == OPEN_MAX)
				fprintf(stderr, "too many clients");

			//client[i].events = POLLRDNORM;
			client[i].events = POLLIN;
			if ( i > maxi)
				maxi = i;

			if( --nready <= 0)
				continue;
		}

		for ( i = 1; i <= maxi; i++){
			if ( (sockfd = client[i].fd) < 0)
				continue;
			if (client[i].revents & (POLLRDNORM | POLLERR)){
					if ( (n = read(sockfd, buf, MAXLINE)) < 0){
						if ( errno = ECONNRESET) {
							close(sockfd);
							client[i].fd = -1;
						}else
							fprintf(stderr, " read error");
					}else if (n == 0){
						close(sockfd);
						client[i].fd = -1;
					}else
						write(sockfd, buf, n);

					if (--nready <= 0)
						break;
			}
		}
	}
}
