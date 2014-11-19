#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <strings.h>

#define SERVPORT 9527
#define MAXBUF 1024
#define MAXFDS 5000
#define EVENTSIZE 100

ssize_t readn(int fd, void *vptr, size_t n)
{
	char *ptr;
	size_t nleft;
	ssize_t nread;

	nleft = n;
	ptr = vptr ;

	while(nleft >0 )
	{
		if((nread = read(fd,ptr,nleft)) == -1)
		{
			if(errno == EINTR )
				continue;
			else if(errno == EAGAIN)
				break;
			else
				return -1;
		}
		else if( nread == 0 )
			break;
		
		nleft -= nread;
		ptr += nread;
	}

	return (n-nleft);
}


int setnonblocking(int fd)
{
	int opts;
	if((opts = fcntl(fd,F_GETFL,0)) == -1)
	{
		perror("fcntl");
		return -1;
	}
	
	opts = opts| O_NONBLOCK;
//	if(( opts =fcntl(fd,F_SETFL, opts)) == -1)
	if(( opts =fcntl(fd,F_SETFL, opts)) == -1)
	{
		perror("fcntl set");
		return -1;
	}

	return 0;
}


int main()
{
	static int count =0;
//	char buf[MAXBUF];
	int len,n;

	struct sockaddr_in servaddr;
	int sockfd,listenfd,epollfd,nfds;

	struct epoll_event ev;
	struct epoll_event events[EVENTSIZE];

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVPORT);

	if( (epollfd = epoll_create(MAXFDS)) == -1)
	{
		perror("epoll");
		exit(1);
	}
	
	if( (listenfd = socket(AF_INET,SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}

	if( setnonblocking(listenfd) == -1)
	{
		perror("setnonblocking");
		exit(1);
	}

	if(bind(listenfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) == -1)
	{
		perror("bind");
		exit(1);
	}

	if(listen(listenfd, 10) == -1)	
	{
		perror("listen");
		exit(1);
	}

	ev.events = EPOLLIN | EPOLLET ;
	ev.data.fd = listenfd;
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD,listenfd,&ev ) == -1 )
	{
		perror("epoll_ctl");
		exit(1);
	}

	for(; ; )
	{
		if( (nfds = epoll_wait(epollfd,events,EVENTSIZE, -1)) == -1)
		{
			perror("epoll_wait");
			exit(1);
		}

 		for( n = 0 ; n < nfds; n++)
		{
			if(events[n].data.fd == listenfd)
			{
//				printf("======befor accept=============\n");
				while( ( sockfd = accept(listenfd, (struct sockadd*) NULL, NULL)) >0 )
//				while(1 )
				{
					struct sockaddr in_addr;
					socklen_t in_len;
					
//					sockfd = accept(listenfd, (struct sockadd*)&in_addr, &in_len);
#if 0
					if(sockfd == -1)
					{
						if((errno == EAGAIN) ||
						    (errno == EWOULDBLOCK ))
							break;
						else
						{
							perror("accept error");
							break;
						}
					}
#endif 

//					sockfd = accept(listenfd, (struct sockadd*) NULL, NULL); 				
#if 0	
					if(sockfd <= 0 )
						printf("sockfd=%d",sockfd);
					else
						printf("sockfd1=%d",sockfd);
#endif 					
					printf("sockfd=%d\n",sockfd);
					if(setnonblocking(sockfd) == -1 )
					{
						perror("set nonblocking");
						exit(1);
					}
					ev.events = EPOLLIN | EPOLLOUT | EPOLLET ;
//					ev.events = EPOLLIN | EPOLLET ;
					ev.data.fd = sockfd;
#if 0				
				event.data.fd = infd;
				event.events = EPOLLIN | EPOLLET;
				s = epoll_ctl(efd,EPOLL_CTL_ADD, infd , &event);
#endif 
//					printf("====epoll_ctl_add===1111==");
					if(epoll_ctl(epollfd, EPOLL_CTL_ADD ,sockfd, &ev) == -1)
					{
						perror("epoll_ctl");
						exit(1);
					}
//					printf("====epoll_ctl_add===2222==");
				}
				//continue;
//				printf("======after accept=============\n");
			}
			else
			{
//				printf("======else=============\n");
				if( events[n].events & EPOLLIN)
				{
#if 0
//					buf[MAXBUF]={0} ;

//					memset((void*)buf, 0,MAXBUF);
					while(1)
					{
						
					}
					char buf[MAXBUF];
					if( (len = readn(events[n].data.fd, buf,MAXBUF)) == -1 )
					{
						if(errno != EAGAIN )
						{
							perror("readn");
							exit(1);
						}
//#if 0
						perror("readn");
						exit(1);
//#endif 
					}
					else if( len == 0)
					{
						printf("=========len=0==============");
						close( events[n].data.fd );
						ev.data.fd = events[n].data.fd;
						ev.events = EPOLLOUT | EPOLLET ;
//						epoll_ctl(epollfd, EPOLL_CTL_MOD, events[n].data.fd ,&ev);
						epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd ,&ev);
					}
					buf[len] = '\0';
					printf("count=%d,received %s\n",count++,buf);

					ev.data.fd = events[n].data.fd;
					//ev.events = EPOLLOUT | EPOLLET ;
					ev.events = EPOLLIN | EPOLLET ;
					epoll_ctl(epollfd, EPOLL_CTL_MOD, events[n].data.fd ,&ev);
#endif
					int n2 = 0 ,nread;
					char buf[MAXBUF];
					memset((void*)buf, 0,MAXBUF);

					printf("-------------------\n");
					while( 1)
					{
//						nread = read(events[n].data.fd, buf+n ,MAXBUF-1);
						nread = read(events[n].data.fd, buf ,MAXBUF-1);
						if(nread == 0)
						{
//							printf("nread = 0 \n");
//							sleep(8000);
//							break;
#if 0
							close(events[n].data.fd);
							ev.data.fd = events[n].data.fd;
							epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
#endif 
						}
//						printf("-----11111111111--------------\n");
						
						if(nread == -1  )
						{
//							printf("--------222222222-----------\n");
							if( errno !=EAGAIN )
							{
								perror("read 9021");
								//exit(1);
								break ;
								//continue;
							}
							else
							{
//								printf("--------333333-----------\n");
//								printf(" errno = EAGAIN ");
								break ;
							}
						}	
//						printf("---------444444444----------\n");
						n2 +=nread;
						int s = write(1, buf, nread);
						//int s = write(1, buf, nread);
						if( s == -1)
						{
							perror("write ");
							abort();
						}
//						printf("---------5555555555----------\n");
					}
//					printf("--=============--\n");
#if 0					
//					printf("--======nread=%d=======--\n",nread);
					int s = write(1, buf, n2);
					if( s == -1)
					{
						perror("write ");
						abort();
					}

					ev.events = EPOLLOUT | EPOLLET ;
					ev.events = EPOLLIN | EPOLLOUT | EPOLLET ;
					epoll_ctl(epollfd, EPOLL_CTL_MOD, events[n].data.fd ,&ev);
#endif					
				}
				if( events[n].events & EPOLLOUT)
				{
					printf("count=%d,when EPOLLIN is triggered, EPOLLOUT is also triggered\n",count++);
#if 0					
					ev.data.fd = events[n].data.fd;
					ev.events = EPOLLIN | EPOLLET ;
					epoll_ctl(epollfd, EPOLL_CTL_MOD, events[n].data.fd ,&ev);
#endif 
				}
			}
//			printf("=======else out=========\n");
		}
	}
}


	

	





			















