#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>

#define SERVPORT 9527
#define MAXBUF 1024
#define MAXFDS 5000
#define EVENTSIZE 100



int main()
{
	char buf[MAXBUF];
	int len,n;

	struct sockaddr_in servaddr;
	int fd ;

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(SERVPORT);
	
	if( (fd = socket(AF_INET,SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}


	if(connect(fd, (struct sockaddr *)&servaddr,sizeof(servaddr)) == -1)
	{
		perror("connect ");
		exit(1);
	}
	
	printf("===========before for============\n");
	for( n = 0 ; n < 10; n++)
	{
		snprintf(buf,MAXBUF, " time %d",n );
		len = strlen(buf);
		printf("buf :%s\n",buf);
		if(write(fd,buf,len) != len)
		{
			printf(" write error");
			close(fd);
			exit(1);
		}
	}
	printf("\n============after for============\n");

	close(fd);
	return 0;
	
}


	

	





			















