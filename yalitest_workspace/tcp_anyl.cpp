
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


using namespace std;
using namespace donson::im;
using namespace google::protobuf;



int main(int argc,char*argv[]){

#if 0
	char buffer[] = {
	0x30, 0x00, 0x0a, 0x00, 0x0c, 0x08, 0x64, 0x10, 
	0x01, 0x18, 0x09, 0x20, 0x0c, 0x28, 0x04, 0x08, 
	0x61, 0x12, 0x08, 0x0a, 0x06, 0x08, 0x00, 0x12, 
	0x02, 0x61, 0x6b, 0x40 };
#endif 

#if 0
	char buffer[] = {
0x30, 0x00, 0x0a, 0x00, 0x0c, 0x08, 0x64, 0x10, 
0x01, 0x18, 0x10, 0x20, 0x0c, 0x28, 0x04, 0x08, 
0x61, 0x12, 0x08, 0x0a, 0x06, 0x08, 0x00, 0x12, 
0x02, 0x6c, 0x74, 0x40 };
#endif 
#if 0
	char buffer[] = {
	0x30, 0x00, 0x0a, 0x00, 0x0b, 0x08, 0x64, 0x10, 
	0x01, 0x18, 0x0f, 0x20, 0x0c, 0x28, 0x04, 0x08, 
	0x61, 0x12, 0x07, 0x0a, 0x05, 0x08, 0x00, 0x12, 
	0x01, 0x77, 0x40 };
#endif 
#if 0
	char buffer[] = {
0x30, 0x00, 0x0a, 0x00, 0x0e, 0x08, 0x64, 0x10, 
0x01, 0x18, 0x0e, 0x20, 0x0c, 0x28, 0x04, 0x08, 
0x61, 0x12, 0x0a, 0x0a, 0x08, 0x08, 0x00, 0x12, 
0x04, 0x31, 0x61, 0x6a, 0x6e, 0x40 };
#endif 

	char buffer[] = {
	0x30, 0x00, 0x0a, 0x00, 0x0d, 0x08, 0x64, 0x10, 
	0x01, 0x18, 0x0d, 0x20, 0x0c, 0x28, 0x04, 0x08, 
	0x61, 0x12, 0x09, 0x0a, 0x07, 0x08, 0x00, 0x12, 
	0x03, 0x61, 0x66, 0x6a, 0x40 };


	int HeaderLen = ntohs(*((unsigned short*)&buffer[1]));
	int PacketLen = ntohs(*((unsigned short*)&buffer[3]));

	ReqHeader header;
	string s("");
	int iRet = (int) header.ParseFromArray(buffer+5, HeaderLen);
	if (!iRet)
	{
		fprintf(stderr, "header.cmd() is unkonwn ");
	}
	else
	{
		fprintf(stderr, "header.cmd()=%d\n",header.cmd());
	}
		
	string packet(buffer+5+HeaderLen, PacketLen);	//packet的位置
	
	PushMsg pushMsg;
	Msg msg;
	RichMsg richMsg;
	
	bool bRet = pushMsg.ParseFromString(packet);

	if (!bRet)
	{
		fprintf(stderr, "pushMsg.ParseFromString(packet) is unkonwn ");
	}
	else
	{

		int fromuin = pushMsg.fromuin();
		msg = pushMsg.msg();
		int message_size = msg.messages_size();
		fprintf(stderr, "msg.messages_size() =%d\n ",message_size);
		// 返回 的repeated RichMsg messages 是一个数组，按照索引取值
		richMsg = msg.messages(0);
		string realStr = richMsg.msg();
		fprintf(stderr, "string realStr =%s\n ",realStr.c_str());
		
	}

#if 0
	message RichMsg 	//定义消息类型
	{
		required MsgType type = 1 [default = Msg_Text]; 	// 消息类型
		required string msg = 2;				// 消息内容
	}
	
	// 消息命令
	message Msg
	{
		repeated RichMsg messages = 1;	// 消息
		optional uint32 msgid = 2;	// 消息下发时使用
		optional uint32 time = 3;	// server校正时间
	}
	
	// 服务器转发聊天消息包
	message PushMsg
	{
		required uint32 fromuin = 1;	// 发送者ID
		required Msg msg = 2;		// 消息
	}
#endif 
	

	return 0;
}







