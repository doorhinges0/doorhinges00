/************************************************************
 * ����(list)
 * �ö��в��漰�ڴ������ͷ�
 * Author: allen
 ***********************************************************/
#ifndef __BS_LIST_H
#define __BS_LIST_H

#include "def.h"

#ifdef	__cplusplus
extern "C" {
#endif

#pragma pack(1)

// ���нڵ�
typedef struct st_list_node
{
	struct st_list_node* 		next;						// ��һ���ڵ�ĵ�ַ
} list_node_t;

#pragma pack()

// ���ȶ���
typedef struct
{
	uint16						off_node;					// Ԫ�ص�ƫ��
	list_node_t*				head;						// ͷ�ڵ�
	list_node_t*				tail;						// δ�ڵ�
	uint64	 					len; 						// ���г���
	uint64						max_len;					// ������󳤶�
	char						key[DEF_KEY_LEN];			// ����
	sem_t						sem;						// �źŵƣ������������
	pthread_mutex_t 			mutex; 						// �߳���
} x_list_t;

#define DEF_LIST_NODE_LEN	sizeof(list_node_t)

// �������в��ҳ�ʼ��
// max_len:������󳤶ȣ�0��ʾ���޳�
// off_node:������е����ݱ������list_node_t����ֵ��ʾ�����ݵ�ƫ��
int x_list_create(x_list_t **ptr, uint64 max_len, uint16 off_node, const char *s);

// ���ٶ���
// n:��ʾҪ���ѵ��߳�(��ʹ������ȡ����ʱ,��Ҫ����)
int x_list_destroy(x_list_t **ptr, int n);

// ���ö���
int x_list_reset(x_list_t *ptr);

// ����������
// data:���ݵ�ַ��level:����
int x_list_push(x_list_t *ptr, void *data);

// �õ�����,��ɾ����Ԫ��,��Ҫ�Լ����ͷŸÿռ�
// flag:true,������ʱ�����ȴ�
void *x_list_pop(x_list_t *ptr, bool flag);

// �õ�����,��ɾ��,���ܹ��ͷ�����ռ�
void *x_list_top(x_list_t *ptr);

// �õ����г���
uint64 x_list_length(x_list_t *ptr);

// �����߳�
void x_list_wakeup(x_list_t *ptr, int n);


#ifdef	__cplusplus
}
#endif

#endif
