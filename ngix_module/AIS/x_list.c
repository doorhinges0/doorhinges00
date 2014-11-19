#include "x_list.h"

// 创建队列并且初始化
// max_len:队列最大长度，0表示无限长
// off_node:插入队列的数据必须包含list_node_t，此值表示该数据的偏移
int x_list_create(x_list_t **ptr, uint64 max_len, uint16 off_node, const char *s)
{
	if (ptr == NULL)
	{
		   fprintf(stderr, "x_list_create failed(NULL)\n");
		return -1;
	}

	*ptr = (x_list_t *)malloc(sizeof(x_list_t));

	if ((*ptr) == NULL)
	{
		   fprintf(stderr, "x_list_create failed(malloc)[%s]\n", strerror(errno));
		return -1;
	}

	(*ptr)->len = 0L;
	(*ptr)->off_node = off_node;
	(*ptr)->head = NULL;
	(*ptr)->tail = NULL;

	if (s != NULL)
	{
		snprintf((*ptr)->key, sizeof((*ptr)->key), "%s", s);
	}
	else
	{
		(*ptr)->key[0] = 0;
	}

	if (max_len == 0L)
	{
		(*ptr)->max_len = ~max_len;
	}
	else
	{
		(*ptr)->max_len = max_len;
	}

	// 初始化信号灯
	if (sem_init(&((*ptr)->sem), 0, 0) != 0)
	{
		free(*ptr);
		*ptr = NULL;
		   fprintf(stderr, "x_list_create failed(sem_init)[%s]\n", strerror(errno));
		return -1;
	}

	// 初始化线程锁
	if (pthread_mutex_init(&((*ptr)->mutex), NULL) != 0)
	{
		sem_destroy(&((*ptr)->sem));
		free(*ptr);
		*ptr = NULL;
		   fprintf(stderr, "x_list_create failed(pthread_mutex_init)[%s]\n", strerror(errno));
		return -1;
	}

	   fprintf(stderr, "x_list_create success[%s]\n", (*ptr)->key);
	return 0;
}

// 销毁队列
int x_list_destroy(x_list_t **ptr, int n)
{
	if (ptr == NULL || (*ptr) == NULL)
	{
		   fprintf(stderr, "x_list_destroy failed(NULL)\n");
		return -1;
	}

	x_list_t *p = *ptr;
	int i;
	n = n <= 0 ? 1 : n;

	if (pthread_mutex_lock(&(p->mutex)) == 0)
	{
		*ptr = NULL;
		p->len = 0L;
		p->head = NULL;
		p->tail = NULL;

		// 唤醒阻塞线程
		for (i = 0; i < n; i++)
		{
			sem_post(&(p->sem));
		}
		sem_destroy(&(p->sem));

		pthread_mutex_unlock(&(p->mutex));
		pthread_mutex_destroy(&(p->mutex));

		   fprintf(stderr, "x_list_destroy success[%s]\n", p->key);
		free(p);

		return 0;
	}
	else
	{
		   fprintf(stderr, "x_list_destroy failed(pthread_mutex_lock)[%s]\n", strerror(errno));
		return -1;
	}
}

// 重置队列
int x_list_reset(x_list_t *ptr)
{
	if (ptr == NULL)
	{
		   fprintf(stderr, "x_list_reset failed(NULL)\n");
		return -1;
	}

	if (pthread_mutex_lock(&(ptr->mutex)) == 0)
	{
		ptr->head = ptr->tail = NULL;
		ptr->len = 0;

		/* BEGIN: modified by yushu/psho2887 at 2012-01-22, for gdb debug */
		while (sem_trywait(&(ptr->sem)) == 0 || errno == EINTR);
		/* while (sem_trywait(&(ptr->sem)) == 0); */
		/* END: modified by yushu/psho2887 at 2012-01-22, for gdb debug */
		pthread_mutex_unlock(&(ptr->mutex));

		fprintf(stderr, "x_list_reset success\n");

		return 0;
	}
	else
	{
		   fprintf(stderr, "x_list_reset failed(pthread_mutex_lock)[%s]\n", strerror(errno));
		return -1;
	}
}

// 插入新数据
// data:数据地址，level:级别
int x_list_push(x_list_t *ptr, void *data)
{
	if (ptr == NULL || data == NULL)
	{
		return -1;
	}

	int status = 0;

	if (pthread_mutex_lock(&(ptr->mutex)) == 0)
	{
		if (ptr->len < ptr->max_len)
		{
			list_node_t *list_node = (list_node_t *)((char *)data + ptr->off_node);
			list_node->next = NULL;
			if (ptr->head == NULL)
			{
				ptr->head = ptr->tail = list_node;
			}
			else
			{
				ptr->tail->next = list_node;
				ptr->tail = list_node;
			}
			ptr->len++;

			if (sem_post(&(ptr->sem)) != 0)
			{
				   fprintf(stderr, "x_list_push failed(sem_post)[%s][%s]\n", ptr->key, strerror(errno));
			}
		}
		else
		{
			status = -1;
		}

		pthread_mutex_unlock(&(ptr->mutex));
	}
	else
	{
		fprintf(stderr, "x_list_push failed(pthread_mutex_lock)[%s]\n", strerror(errno));
		status = -1;
	}

	return status;
}

// 得到数据,且删除该元素,需要自己来释放该空间
// flag:true,无数据时阻塞等待
void *x_list_pop(x_list_t *ptr, bool flag)
{
	if (ptr == NULL)
	{
		return NULL;
	}

	void *data = NULL;

	if (flag)
	{
		/* BEGIN: modified by yushu/psho2887 at 2012-01-22, for gdb debug */
		while(sem_wait(&(ptr->sem)) != 0)
		{
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				   fprintf(stderr, "x_list_pop failed(sem_wait)[%s][%s]\n", ptr->key, strerror(errno));
				return NULL;
			}
		}
		/*
		if (sem_wait(&(ptr->sem)) != 0)
		{
			xLog(XLOG_FATAL, "x_list_pop failed(sem_wait)[%s][%s]", ptr->key, strerror(errno));
			return NULL;
		}
		*/
		/* END: modified by yushu/psho2887 at 2012-01-22 */
	}

	if (pthread_mutex_lock(&(ptr->mutex)) == 0)
	{
		if (ptr->head != NULL) 
		{
			data = (void *)((char *)(ptr->head) - ptr->off_node);
			ptr->len--;

			ptr->head = ptr->head->next;
		}

		pthread_mutex_unlock(&(ptr->mutex));
	}
	else
	{
		   fprintf(stderr, "x_list_pop failed(pthread_mutex_lock)[%s]\n", strerror(errno));
	}

	return data;
}

// 得到数据,不删除,不能够释放这个空间
void *x_list_top(x_list_t *ptr)
{
	if (ptr == NULL)
	{
		return NULL;
	}

	void *data = NULL;

	if (pthread_mutex_lock(&(ptr->mutex)) == 0)
	{
		if (ptr->head != NULL)
		{
			data = (void *)((char *)(ptr->head) - ptr->off_node);
		}
		pthread_mutex_unlock(&(ptr->mutex));
	}
	else
	{
		   fprintf(stderr, "x_list_top failed(pthread_mutex_lock)[%s]\n", strerror(errno));
	}

	return data;
}

// 得到队列长度
uint64 x_list_length(x_list_t *ptr)
{
	if (ptr == NULL)
	{
		return 0L;
	}

	uint64 len = 0L;

	if (pthread_mutex_lock(&(ptr->mutex)) == 0)
	{
		len = ptr->len;
		pthread_mutex_unlock(&(ptr->mutex));
	}
	else
	{
		   fprintf(stderr, "x_list_length failed(pthread_mutex_lock)[%s]\n", strerror(errno));
	}

	return len;
}

// 唤醒线程
void x_list_wakeup(x_list_t *ptr, int n)
{
	if (ptr == NULL)
	{
		return;
	}

	n = n <= 0 ? 1 : n;

	int i;

	if (pthread_mutex_lock(&(ptr->mutex)) == 0)
	{
		for (i = 0; i < n; i++)
		{
			if (sem_post(&(ptr->sem)) != 0)
			{
				   fprintf(stderr, "x_list_wakeup failed(sem_post)[%s][%s]\n",
					ptr->key, strerror(errno));
			}
		}
		pthread_mutex_unlock(&(ptr->mutex));

		   fprintf(stderr, "x_list_wakeup success[%s]\n", ptr->key);
	}
	else
	{
		   fprintf(stderr, "x_list_wakeup failed(pthread_mutex_lock)[%s]\n", strerror(errno));
	}
}
