/************************************************************
 * 队列(list)
 * 该队列不涉及内存分配和释放
 * Author: allen
 ***********************************************************/
#ifndef __BS_LIST_H
#define __BS_LIST_H

#include "def.h"

#ifdef	__cplusplus
extern "C" {
#endif

#pragma pack(1)

// 队列节点
typedef struct st_list_node
{
	struct st_list_node* 		next;						// 下一个节点的地址
} list_node_t;

#pragma pack()

// 优先队列
typedef struct
{
	uint16						off_node;					// 元素的偏移
	list_node_t*				head;						// 头节点
	list_node_t*				tail;						// 未节点
	uint64	 					len; 						// 队列长度
	uint64						max_len;					// 队列最大长度
	char						key[DEF_KEY_LEN];			// 键名
	sem_t						sem;						// 信号灯，控制休眠与否
	pthread_mutex_t 			mutex; 						// 线程锁
} x_list_t;

#define DEF_LIST_NODE_LEN	sizeof(list_node_t)

// 创建队列并且初始化
// max_len:队列最大长度，0表示无限长
// off_node:插入队列的数据必须包含list_node_t，此值表示该数据的偏移
int x_list_create(x_list_t **ptr, uint64 max_len, uint16 off_node, const char *s);

// 销毁队列
// n:表示要唤醒的线程(当使用阻塞取数据时,需要唤醒)
int x_list_destroy(x_list_t **ptr, int n);

// 重置队列
int x_list_reset(x_list_t *ptr);

// 插入新数据
// data:数据地址，level:级别
int x_list_push(x_list_t *ptr, void *data);

// 得到数据,且删除该元素,需要自己来释放该空间
// flag:true,无数据时阻塞等待
void *x_list_pop(x_list_t *ptr, bool flag);

// 得到数据,不删除,不能够释放这个空间
void *x_list_top(x_list_t *ptr);

// 得到队列长度
uint64 x_list_length(x_list_t *ptr);

// 唤醒线程
void x_list_wakeup(x_list_t *ptr, int n);


#ifdef	__cplusplus
}
#endif

#endif
