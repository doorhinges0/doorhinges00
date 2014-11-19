
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h> 
#include <sys/stat.h>
#include <fcntl.h>

#include <string>
#include <iostream>
#include "donsonim.pb.h"
#include "sock.h"
//#include "../../comm/credis/credis.h"
//#include "../../comm/comm/code_conv.h"
//#include "../../comm/comm/sock.h"

//REDIS g_stRedis; 
using namespace std;
using namespace donson::im;
using namespace google::protobuf;
using namespace network;

//#define Max_Statics_Group_Count  100
//#define MAX_MSG_LEN 9612

//int port =0;

//int staticgroup[Max_Statics_Group_Count] = {0};
int main(int argc,char*argv[]){

	unsigned int TEST_NUM = 7000;
	
for ( unsigned int i = 0 ; i < TEST_NUM;  i++)
{

	cout<< "i="<<i<<endl;
	
	pid_t child_pid = fork ();
	if (child_pid < 0)
		perror ("fork:"), exit(1);
	else if (child_pid == 0)
	{
		
	CSocket *p =new CSocket();
	string Add= "127.0.0.1";	
	p->create(AF_INET ,SOCK_STREAM);
	p->connect(Add,806,2);

	string packet = "",slog;
	ReqHeader req;
	req.set_uin(10001);
	req.set_appid(10000);
	//req.set_appid("LQnZPsjF2x7OIM45NuXfu5");
	//req.set_apptoken("LXQ34vfxyh9rqIUVtlgef6");
	req.set_seq(1);
	req.set_cmd(CMD_LOGIN); // 1 means login
	req.set_ver(1);
	req.SerializeToString(&packet);
	
	char buff[32]= {0};
	ReqLogin login;
//	login.set_devicetoken("1234567890");
//	login.set_logintoken("123456");
//	login.set_devicetype(1);

	login.set_username("11111111");
    login.set_password("11111111");
   	login.set_devicetoken("11111111");
   	login.set_devicetype(1);

	login.SerializeToString(&slog);

	string slogin = "";

	slogin.clear();
	slogin+=0x30;
	unsigned short headerlen = htons(packet.size()); 
	unsigned short bodylen = htons(slog.size()); 
	slogin.append((char*)&headerlen,sizeof(unsigned short)); 
	slogin.append((char*)&bodylen,sizeof(unsigned short)); 
	slogin.append(packet); 
	slogin.append(slog); 
	slogin+=0x40;
	int len = packet.size() + slog.size() + 6;
	cout<<"send len="<<p->send(slogin.c_str(),len,0)<<endl;

	char recvbuff[1024] = {0};
	p->receive(recvbuff,1024,0);
	RspHeader header;
	int HeaderLen = ntohs(*((unsigned short*)&recvbuff[1]));
	int PacketLen = ntohs(*((unsigned short*)&recvbuff[3]));
	string s("");
	int iRet = (int) header.ParseFromArray(recvbuff+5,HeaderLen);
	cout<<header.ret()<<"@@@@@"<<header.message()<<endl;

	RspLogin rsp;
	iRet = (int)rsp.ParseFromArray(recvbuff +5 + HeaderLen, PacketLen);
	cout<<rsp.token()<<"---"<<rsp.apiresponse()<<endl;

	for(; ; )
    {
    	sleep(50*1);
    	cout<< "sleep(1000*1)"<<endl;
			
    	ReqHeader reqheader;
	    reqheader.set_uin(800002); //  800000 start 
	   	reqheader.set_appid(0);
	   	reqheader.set_seq(0);
	   	reqheader.set_cmd(CMD_HEARTBEAT); // 3 means CMD_HEARTBEAT
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
	   	
	   	string headlen1="";
	    string bodylen1="";
	    
	    headlen1.append(buff1);
	    bodylen1.append(buff2);

	   
	   	string sPacket = "\x30" + headlen1 + bodylen1   +  rhPacket + rlPacket + "\x40";
	    int 	buflength= sPacket.length();

		


		string slogin = "";
		
		slogin.clear();
		slogin+=0x30;
		unsigned short headerlen = htons(rhPacket.size()); 
		unsigned short bodylen = htons(rlPacket.size()); 
		slogin.append((char*)&headerlen,sizeof(unsigned short)); 
		slogin.append((char*)&bodylen,sizeof(unsigned short)); 
		slogin.append(rhPacket); 
		slogin.append(rlPacket); 
		slogin+=0x40;

		int len = rhPacket.size() + rlPacket.size() + 6;


		cout<< "p->send "<<endl;
		
	//	p->send(sPacket.c_str(), buflength , 0);
		cout<<"send2 len="<<p->send(slogin.c_str(), len , 0)<<endl;
	   	//send(client_sockfd,buf,buflength,0);
    	
    }

	#if 0
	char recvbuff[1024] = {0};
	p->receive(recvbuff,1024,0);
	RspHeader header;
	int HeaderLen = ntohs(*((unsigned short*)&recvbuff[1]));
	int PacketLen = ntohs(*((unsigned short*)&recvbuff[3]));
	string s("");
	int iRet = (int) header.ParseFromArray(recvbuff+5,HeaderLen);
	cout<<header.ret()<<"@@@@@"<<header.message()<<endl;
	#endif 
		}
	
}
	return 0;
}







