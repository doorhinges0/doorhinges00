#ifndef __HEARTBEAT_H__
#define __HEARTBEAT_H__

#define MSG_TYPE1	1
#define MSG_TYPE2	2
#define MSG_HEARTBEAT   3

typedef struct
{
	u_int32_t type;
	char data[2000];
} msg_t;

#define T1		60
#define T2		10

#endif 

