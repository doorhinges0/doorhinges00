#ifndef VLSA_COMMON_H
#define VLSA_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include <endian.h>
#include <ctype.h>
#include <termios.h>
#include <sys/resource.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/time.h>


#include "db.h"
#include "msg_fifo.h"
#include "vlsa_msg.h"
#include "vlsa_retransor.h"
#include "vlsa_switch.h"
#include "OSAL.h"
#ifdef INCLUDE_MON
#include "monitor.h"
#endif

//#define VERSION_TAG		"1.00.01"
#define COMPILE_VERSION	"*** VLS-APP "VERSION" Built on " __DATE__ " at " __TIME__" ***"

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x,y) ((x)>(y) ? (x) : (y))
#endif


#define	RET_OK				0
#define RET_ERR				-1
#define RET_OTHER			1

#define MAX_IP_LEN			16


#define MAX_TIMER_NUM		10
#define MAX_DB_CONN_NUM		20

#define VLSA_ASYN_TIMER_CNT 50

#define VLSA_CFG_FILE		"/root/.vlsa/profile/vlsa.conf"

#define LOG_PATH			"/usr/local/var/log/vlsa/"


typedef struct listen_info
{
	char ip[MAX_IP_LEN];
	short port;
	int sock;
	
}listen_info_t;


//return:RET_OK/RET_ERR
typedef int (*workthreads_cb)(void *);


typedef struct msg_queue_params
{
	int max_fifo_nodes;				//need init
	free_node_f free_node_func;		//need init
	
	int max_workthreads_num;		//need init
	workthreads_cb cb;				//need init, use to deal msg
	
	pthread_mutex_t fifo_mutex;		
	msg_fifo_t *fifo;				
	
	sem_t semaphore;				
	pthread_t *workthread_id_list;	
	
}msg_queue_params_t;




//return:RET_OK/RET_ERR
int msg_enqueue(msg_queue_params_t *p,void *pmsg);



//return:RET_OK/RET_ERR
int msq_thread_pool_init(msg_queue_params_t *p);


void msq_thread_pool_end(msg_queue_params_t *p);



//sock_type: SOCK_STREAM/SOCK_DGRAM
//proto: IPPROTO_TCP/IPPROTO_UDP
//return:RET_ERR/socket value
int create_sock(char *ip, unsigned short port,int sock_type,int proto);


//return:RET_ERR/RET_OK
int udp_send(int sock,const char *pbuf,int len,unsigned int ipvalue,unsigned short port);


//return: RET_OK/RET_ERR
int	tcp_send(int sock,const char *pbuf,int len);


//in:	 time:seconds 
//out:	pstrbuf:string buf of the time change (buf size must more than 30 bytes):"%Y-%m-%d %H:%M:%S"
void time_int_to_str(char *pstrbuf, unsigned int time);


void set_core_dump();


#ifdef __cplusplus
}
#endif


#endif

