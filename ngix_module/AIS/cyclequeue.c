/*
 *       Filename:  cyclequeue.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/25/2013
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan Chen, ivanchen36@gmail.com
 *   Organization:  
 */
#include <stdlib.h>
#include <string.h>
#include "cyclequeue.h"

eg_cycle_queue_t *eg_cq_create(int size, int val_size)
{
    eg_cycle_queue_t *cq;
    eg_cg_bucket_t *bt;
    eg_cq_node_t *node;
    unsigned char *buf = NULL;

    cq = (eg_cycle_queue_t *)malloc(sizeof(eg_cycle_queue_t));
    if (!cq) return NULL;
    
    bt = (eg_cg_bucket_t *)malloc(sizeof(eg_cg_bucket_t));
    if (!bt )
    {
        free(cq);

        return NULL;
    }

    node = (eg_cq_node_t *)malloc(size * sizeof(eg_cq_node_t));
    if (!node)
    {
        free(cq);
        free(bt);

        return NULL;
    }

    if(val_size > 0)
    {
        buf = (unsigned char *)malloc(size * val_size);
        if(!buf)
        {
            free(cq);
            free(bt);
            free(node);

            return NULL;
        }
    }

    eg_cq_initbucket(node, size, buf, val_size);
    node[size - 1].next = node;

    bt->node = node;
    bt->head = NULL;
    bt->next = NULL;
    bt->buf = buf;
    bt->size = size;

    cq->head = node;
    cq->tail = node;
    cq->btlist = bt;
    cq->val_size = val_size;
    cq->size = size;
    cq->elements = 0;
    cq->bt_index = 0;

    return cq;
}

void eg_cq_destroy(eg_cycle_queue_t *cq)
{
    if(!cq) return;

    eg_cg_bucket_t *bt;
    eg_cg_bucket_t *tmp;

    for(bt = cq->btlist; bt; )
    {
        free(bt->node); 
        if(bt->buf) free(bt->buf); 
        tmp = bt;
        bt = bt->next;
        free(tmp); 
    }
    free(cq);
    cq = NULL;
}

int eg_cq_capity(eg_cycle_queue_t *cq)
{
    if(!cq) return 0;

    return cq->size;
}

int eg_cq_empty(eg_cycle_queue_t *cq)
{
    if(!cq) return 0;

    return cq->head == cq->tail;
}

int eg_cq_push(eg_cycle_queue_t *cq, void *val)
{
    if(!cq) return 0;

    eg_cq_node_t *cur = cq->tail;
    
    if(cur->next == cq->head)
    {
        if(!eg_cq_inc(cq)) return 0;
    }else if(cq->bt_index)
    {
        eg_cg_bucket_t *bt = cq->btlist;
        int end = bt->size -1;

        if(!bt->node->val && !bt->node[end].val 
            && (cur < bt->node || cur > bt->node + end))
        {
            eg_cq_dec(cq);
        }
    }

    cur->val = val;
    cq->tail = cur->next;

    return 1;
}

int eg_cq_inc(eg_cycle_queue_t *cq)
{
    eg_cg_bucket_t *bt;
    eg_cq_node_t *node;
    eg_cq_node_t *cur;
    int size;
    int val_size = cq->val_size;
    unsigned char *buf = NULL;

    size = cq->btlist->size;
    bt = (eg_cg_bucket_t *)malloc(sizeof(eg_cg_bucket_t));
    if (!bt)
    {
        return 0;
    }

    node = (eg_cq_node_t *)malloc(size * sizeof(eg_cq_node_t));
    if (!node)
    {
        free(bt);

        return 0;
    }

    if(val_size > 0)
    {
        buf = (unsigned char *)malloc(size * val_size);
        if(!buf)
        {
            free(bt);
            free(node);

            return 0;
        }
    }

    eg_cq_initbucket(node, size, buf, val_size);
    cur = cq->tail;
    node[size - 1].next = cur->next;
    cur->next = node;

    bt->head = cur;
    bt->node = node;
    bt->next = cq->btlist;
    bt->buf = buf;
    bt->size = size;

    cq->btlist = bt;
    cq->size += size;
    ++cq->bt_index;

    return 1;
}

void eg_cq_dec(eg_cycle_queue_t *cq)
{
    eg_cg_bucket_t *bt;

    bt = cq->btlist;
    cq->btlist = bt->next;
    bt->head->next = bt->node[bt->size - 1].next;
    cq->size -= bt->size;
    --cq->bt_index;
    if(bt->buf) free(bt->buf);
    free(bt->node);
    free(bt);
}

void *eg_cq_pop(eg_cycle_queue_t *cq)
{
    if(eg_cq_empty(cq)) return NULL;

    void *val = cq->head->val;

    cq->head->val = NULL;
    cq->head = cq->head->next;

    return val;
}

void eg_cq_initbucket(eg_cq_node_t *arr, int size, unsigned char *buf, int val_size)
{
    int i;

    memset((void *)arr, 0, sizeof(eg_cq_node_t) * size);
    --size;
    if(buf)
    {
        for(i = 0; i < size; ++i)
        {
            arr[i].next = arr + i + 1;
            arr[i].buf = buf + val_size * i; 
        }
        arr[i].buf = buf + val_size * i; 
        
        return;
    }

    for(i = 0; i < size; ++i)
    {
        arr[i].next = arr + i + 1;
    }
}

void *eg_cq_get_cur_buf(eg_cycle_queue_t *cq)
{
    if(!cq) return NULL;

    return cq->tail->buf;
}

void *eg_cq_head(eg_cycle_queue_t *cq)
{
    if(eg_cq_empty(cq)) return NULL;

    return cq->head->val;
}
