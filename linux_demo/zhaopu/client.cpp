#include <stdio.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <string.h>
#include <stdlib.h>

#include "donsonim.pb.h"

using namespace std;
using namespace donson::im;

#include <google/protobuf/text_format.h>
using namespace google::protobuf;
 
#define BUFFER_SIZE 1024

int main(int argc, char *argv[])   
{   
    int client_sockfd;   
    int len;   
    struct sockaddr_in remote_addr; // 服务器端网络地址结构体   
    char buf[BUFFER_SIZE];  // 数据传送的缓冲区   
    memset(&remote_addr,0,sizeof(remote_addr)); // 数据初始化--清零   
    remote_addr.sin_family=AF_INET; // 设置为IP通信   
    remote_addr.sin_addr.s_addr=inet_addr("112.95.238.67");// 服务器IP地址   
    remote_addr.sin_port=htons(806); // 服务器端口号   
    // 创建客户端套接字--IPv4协议，面向连接通信，TCP协议 
    if((client_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)   
    {   
        perror("client socket creation failed");   
        exit(EXIT_FAILURE);
    }   
    // 将套接字绑定到服务器的网络地址上 
    if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)   
    {   
        perror("connect to server failed");   
        exit(EXIT_FAILURE);
    }
    
    fprintf(stderr, " connect success");
    
    ReqLogin  reqlogin;
    reqlogin.set_username("11111111");
    reqlogin.set_password("11111111");
   	reqlogin.set_devicetoken("11111111");
   	reqlogin.set_devicetype(1);
    string rlPacket;
   	reqlogin.SerializeToString(&rlPacket);
    
    ReqHeader reqheader;
    reqheader.set_uin(0);
   	reqheader.set_appid(0);
   	reqheader.set_seq(0);
   	reqheader.set_cmd(1); // 1 means login
   	reqheader.set_ver(0);
   	
   	string rhPacket;
   	reqheader.SerializeToString(&rhPacket);
   	
   	char buff1[32] = {0};
    sprintf(buff1,"%d",rhPacket.length());
    
    char buff2[32] = {0};
    sprintf(buff2,"%d",rlPacket.length());
    
 //   string headlen= string(buff1);
 //   string bodylen = string(buff2);
    
    string headlen="";
    string bodylen="";
    
    headlen.append(buff1);
    bodylen.append(buff2);
   
  // sret.append(buffi);
   	//string sPacket = "\x30" + rhPacket.length() + rlPacket.length()   +  rhPacket + rlPacket + "\x40";
   	string sPacket = "\x30" + headlen + bodylen   +  rhPacket + rlPacket + "\x40";
   	//buf = sPacket.c_str();
    int 	buflength= sPacket.length();
   	
   	
   	fprintf(stderr, " send before ");
   	
   	memcpy(buf, sPacket.c_str(), sPacket.size());
   	
   	send(client_sockfd,buf,buflength,0);
   	
   	fprintf(stderr, " send  after ");
    
    for(; ; )
    {
    	sleep(30);
    	
    	ReqHeader reqheader;
	    reqheader.set_uin(0);
	   	reqheader.set_appid(0);
	   	reqheader.set_seq(0);
	   	reqheader.set_cmd(3); // 3 means CMD_HEARTBEAT
	   	reqheader.set_ver(0);
	   	
	   	string rhPacket;
	   	reqheader.SerializeToString(&rhPacket);
	   	
	   	ReqHeartBeat rhb;
	   	rhb.set_token("11111111");
	    string rlPacket;
	   	rhb.SerializeToString(&rlPacket);
	   	
	   	char buff1[32] = {0};
	    sprintf(buff1,"%d",rhPacket.length());
	    
	    char buff2[32] = {0};
	    sprintf(buff2,"%d",rlPacket.length());
	   	
	   	string headlen="";
	    string bodylen="";
	    
	    headlen.append(buff1);
	    bodylen.append(buff2);
	   
	   	string sPacket = "\x30" + headlen + bodylen   +  rhPacket + rlPacket + "\x40";
	    int 	buflength= sPacket.length();
	   	
	   	send(client_sockfd,buf,buflength,0);
    	
    }
    

    return 0;
}

