#ifndef SHM_QUEUE_H
#define SHM_QUEUE_H
#include "common.h"

union semun{
	int val;                  /* value for SETVAL */
	struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
	unsigned short *array;    /* array for GETALL, SETALL */
	                          /* Linux specific part: */
	struct seminfo *__buf;    /* buffer for IPC_INFO */
};

#pragma pack(1)
struct queue_header
{
		volatile int head;
		volatile int tail;
};

struct shm_block
{
	int length;
	// 32(ip)+16(port)+16(socketfd)
	int id;	//cid
	
	int type;
	
	char* data;
};
#pragma pack()

#if 0 
struct shm_block
{
	int length;
	// 32(ip)+16(port)+16(socketfd)
	int64_t id;	//cid
	unsigned char protocol;	//Э������
	/*
	* 0: extern call request
	* 1: extern call response
	* 2: intern call request
	* 3: intern call response
	* 4: fill bytes, discard
	*/
	int64_t sockinfo;
	char type;
	char flag;  //note here close flag :0 mean send and close fd, 1 mean only send,3 mean send and close but svc no clean
	struct timeval stTv;
	char* data;
};
#endif 

#define MB_HEAD_LENGTH	((int)(sizeof (shm_block) - sizeof (char*)))
#define MB_DATA_LENGTH(mb)	(mb->length - MB_HEAD_LENGTH)

#define  PAD_REQUEST  7 
static bool stopped = false;

class Shm_Queue
{
public:
		Shm_Queue();
		~Shm_Queue();
		int create (key_t sem, int shm_len);
		int push (const shm_block* data, bool locked, bool blocked);
		int pop ( shm_block* data, bool locked, bool blocked);		

protected:
		int release ();
		int acquire ();
		
		bool check_mb (const shm_block* mem_area);
		int pop_wait ( bool blocked);
		int push_wait ( const shm_block* mb, bool blocked );
		int align_queue_head ( const shm_block* mb);
		int align_queue_tail();
		
		inline shm_block* get_head_mb ()
		{
				return (shm_block*)((char*)shm_addr + shm_addr->head);
		}
		
		inline shm_block* get_tail_mb()
		{
				return (shm_block*)((char*)shm_addr + shm_addr->tail);
		}
		
	int sem_id;
	int shm_len;
	
queue_header* shm_addr;
};

//extern Shm_Queue send_shmq;
//extern Shm_Queue recv_shmq;

#endif
	
				
	