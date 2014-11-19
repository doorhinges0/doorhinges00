#include <stdio.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <string.h>
#include <stdlib.h>
 
#define BUFFER_SIZE 1024

int main(int argc, char *argv[])   
{   
    int client_sockfd;   
    int len;   
    struct sockaddr_in remote_addr; // �������������ַ�ṹ��   
    char buf[BUFFER_SIZE];  // ���ݴ��͵Ļ�����   
    memset(&remote_addr,0,sizeof(remote_addr)); // ���ݳ�ʼ��--����   
    remote_addr.sin_family=AF_INET; // ����ΪIPͨ��   
    remote_addr.sin_addr.s_addr=inet_addr("127.0.0.1");// ������IP��ַ   
    remote_addr.sin_port=htons(8000); // �������˿ں�   
    // �����ͻ����׽���--IPv4Э�飬��������ͨ�ţ�TCPЭ�� 
    if((client_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)   
    {   
        perror("client socket creation failed");   
        exit(EXIT_FAILURE);
    }   
    // ���׽��ְ󶨵��������������ַ�� 
    if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)   
    {   
        perror("connect to server failed");   
        exit(EXIT_FAILURE);
    }  
    // ѭ���������������� 	
    while(1)
	{
		printf("Please input the message:\n");
		//scanf("%s",buf);
		
		fgets(buf, sizeof(buf), stdin);
		// exit
		if(strcmp(buf,"exit")==0)
		{
			break;
		}
		send(client_sockfd,buf,BUFFER_SIZE,0);
		// ���շ���������Ϣ 
	 	len=recv(client_sockfd,buf,BUFFER_SIZE,0);
		printf("receive from server:%s\n",buf);
		if(len<0)
		{
			perror("receive from server failed");
			exit(EXIT_FAILURE);
		}
	}
    //close(client_sockfd);// �ر��׽���   
    return 0;
}
