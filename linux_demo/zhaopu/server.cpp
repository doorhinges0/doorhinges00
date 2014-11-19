#include <stdio.h>   
#include <sys/types.h>   
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include<iostream>
#include<fcntl.h>
#include <errno.h>



#define BUFFER_SIZE 1024
#define MAX_EVENTS 100

void setnonblock(int sockfd)
{
	int opts = fcntl(sockfd,F_GETFL);
	if(opts < 0)
	{
		perror("fcntl");
	}
	opts = opts|O_NONBLOCK;
	fcntl(sockfd,F_SETFL,opts);
}

int main(int argc, char * argv[])   
{
    int server_sockfd;// ���������׽���   
    int client_sockfd;// �ͻ����׽���   
    int len;   
    struct sockaddr_in my_addr;   // �����������ַ�ṹ��   
    struct sockaddr_in remote_addr; // �ͻ��������ַ�ṹ��   
    socklen_t sin_size;   
    char buf[BUFFER_SIZE];  // ���ݴ��͵Ļ�����   
    memset(&my_addr,0,sizeof(my_addr)); // ���ݳ�ʼ��--����   
    my_addr.sin_family=AF_INET; // ����ΪIPͨ��   
    my_addr.sin_addr.s_addr=INADDR_ANY;// ������IP��ַ--�������ӵ����б��ص�ַ��   
    my_addr.sin_port=htons(8000); // �������˿ں�   
    // �������������׽���--IPv4Э�飬��������ͨ�ţ�TCPЭ��
    if((server_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)   
    {     
        perror("socket");   
        return 1;   
    }   
    // ���׽��ְ󶨵��������������ַ��
    if (bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)   
    {   
        perror("bind");   
        return 1;   
    }   
    // ������������--�������г���Ϊ5 
    listen(server_sockfd,5);
    setnonblock(server_sockfd);
    sin_size=sizeof(struct sockaddr_in); 
	// ����һ��epoll���
	int epoll_fd;
	epoll_fd=epoll_create(MAX_EVENTS);
	if(epoll_fd==-1)
	{
		perror("epoll_create failed");
		exit(EXIT_FAILURE);
	}
   	struct epoll_event ev;// epoll�¼��ṹ��
	struct epoll_event events[MAX_EVENTS];// �¼���������
//	ev.events=EPOLLIN;
	ev.events = EPOLLIN | EPOLLET ;
	ev.data.fd=server_sockfd;
	// ��epollע��server_sockfd�����¼�
	if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,server_sockfd,&ev)==-1)
	{
		perror("epll_ctl:server_sockfd register failed");
		exit(EXIT_FAILURE);
	}
	int nfds;// epoll�����¼������ĸ���
    // ѭ�����ܿͻ�������	
    while(1)
	{
		// �ȴ��¼�����
		nfds=epoll_wait(epoll_fd,events,MAX_EVENTS,-1);
		if(nfds==-1)
		{
			perror("start epoll_wait failed");
			exit(EXIT_FAILURE);
		}
		int i;
		for(i=0;i<nfds;i++)
		{
			printf("===========================nfds = %d\n",nfds);
			// �ͻ������µ���������
			if(events[i].data.fd==server_sockfd)
			{
				// �ȴ��ͻ����������󵽴�
        		if((client_sockfd=accept(server_sockfd,(struct sockaddr *)&remote_addr,&sin_size))<0)
				{   
					perror("accept client_sockfd failed");   
					exit(EXIT_FAILURE);
				}
				
				if(client_sockfd == -1)
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
				
				
				// ��epollע��client_sockfd�����¼�
			        setnonblock(client_sockfd);
	//		        ev.events=EPOLLIN;
								ev.events = EPOLLIN | EPOLLOUT | EPOLLET ;
				ev.data.fd=client_sockfd;
				if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_sockfd,&ev)==-1)
				{
					perror("epoll_ctl:client_sockfd register failed");
					exit(EXIT_FAILURE);
				}
				char * str = inet_ntoa(remote_addr.sin_addr);
				std::cout<<"accpet client IP :"<<str<<std::endl;
				//printf("accept client %s/n",inet_ntoa(remote_addr.sin_addr));
				
				int n2 = 0 ,nread;
					char buf[BUFFER_SIZE];
					memset((void*)buf, 0,BUFFER_SIZE);

					printf("-------------------\n");
					while( 1)
					{
//						nread = read(events[n].data.fd, buf+n ,MAXBUF-1);
						nread = read(events[i].data.fd, buf ,BUFFER_SIZE-1);
						if(nread == 0)
						{
//							printf("nread = 0 \n");
//							sleep(8000);
//							break;
#if 0
							close(events[i].data.fd);
							ev.data.fd = events[n].data.fd;
							epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
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


				
				
			}
			// �ͻ��������ݷ��͹���
//			else
			if ( events[i].data.fd & EPOLLOUT)
			{
				
				ev.data.fd = events[i].data.fd;
				ev.events = EPOLLIN | EPOLLET ;
				epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd ,&ev);
				
			}
		}
	}
	return 0;   
}  