
#include "Shm_Queue.h"
#include <string>
using namespace std;


void printMB( shm_block* mb)
{
	/*char data[20] ={0};
 
	printf("%d\n",mb->type );
	printf("%d\n", mb->id );
	

//	memcpy (mb->data, (char*)tail_mb + MB_HEAD_LENGTH, MB_DATA_LENGTH (tail_mb));	
	memcpy (data, (char*)mb + MB_HEAD_LENGTH, MB_DATA_LENGTH (mb));	
	printf("%s\n", data);
	*/
	
	char data[20] ={0};
	//printf("========== printMB==========\n");
	printf("%d\n",mb->type );
	printf("%d\n", mb->id );
	
	//memcpy (data, (char*)mb + MB_HEAD_LENGTH, MB_DATA_LENGTH (mb));
	//data[MB_DATA_LENGTH (mb)]='\0';	
	memcpy (data, mb->data, MB_DATA_LENGTH (mb));
	fprintf(stderr, "@@@@@data:%s@@@@@\n", data);
	//printf("*****%s*****\n", data);
	
		//printf("========== printMB==========\n");
}


int main(int argc, char *argv[])
{
	Shm_Queue recv_shmq;
	
	int recv_sem_key = 666666;
	int shm_queue_length = 10240;
	recv_shmq.create (recv_sem_key, shm_queue_length);
	
	
	
	int max_child_count = 2;
	
	pid_t pid;
	
	char* buffer ="_qqqqq";
	static char* buffer2 ="_wwwww";
	static char* buffer3="qwertyuiop";
	#if 1 
	if ((pid = fork()) < 0 )
	{
		fprintf(stderr, "fork error\n");
	}else if (pid > 0)
	#endif
	{
		/*
		shm_block mb;
		mb.length = 10 + MB_HEAD_LENGTH;
		//mb.length = buffer.length() + MB_HEAD_LENGTH;
	//	mb.data = (char*)buffer2.c_str();
		mb.data = buffer3;
		mb.type = 11;
		mb.id = 22;
		
		recv_shmq.push(&mb, false, false);
		fprintf(stderr, " push finished in main\n");
		*/
		//recv_shmq.push(&mb, false, false);
		//
		#if 1
		unsigned int j = 0;
		unsigned int id = 0;
	/*	
		for ( j = 0 ; j < max_child_count ; j++)
		{
			
			shm_block mb;
			mb.length = 6 + MB_HEAD_LENGTH;
			if (j == 0) 
				mb.data= buffer;
			else
				mb.data = buffer2;
			mb.type = 11;
			mb.id = id++;
			
			fprintf(stderr, "*****j=%d*****\n", j);
			recv_shmq.push(&mb, false, false);
			
			//recv_shmq.push(&mb, false, false);
			
		}*/
		
		for ( j = 0 ; j < max_child_count ; j++)
		{
			
		
			
			//recv_shmq.push(&mb, false, false);
			if ((pid = fork()) < 0 )
			{
				fprintf(stderr, "fork error\n");
			}else if (pid > 0)
			{
					shm_block mb;
					mb.length = 6 + MB_HEAD_LENGTH;
					if (j == 0) 
						mb.data= buffer;
					else
						mb.data = buffer2;
					mb.type = 11;
					mb.id = id++;
					
					fprintf(stderr, "*****j=%d*****\n", j);
					recv_shmq.push(&mb, true, true);
			}
			else
			{
				
			}
			
		}
		
		
		#endif 
		//
	}
//	#if 1 
	else
	{
		sleep(9);
		shm_block mb;
		if ((mb.data = (char*) malloc (512)) == NULL)
		{
			fprintf(stderr, "malloc error\n");;
		  return -1 ;
		}
	  memset(mb.data, 0, sizeof(char) * 512);
	//	recv_shmq.pop (&mb, false, false);
		for (; recv_shmq.pop (&mb, true, true) == 0 ;)
				printMB (&mb);
		
		
	//	send_shmq.pop (&mb, true, true);
		
	}
//	#endif

	return 0;
	
	
}





