/*
 *       Filename:  cyclequeu.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/24/2013
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan Chen, ivanchen36@gmail.com
 *   Organization:  
 */

#ifndef _CYCLEQUEUE_H_
#define _CYCLEQUEUE_H_

typedef struct eg_cq_node
{
    struct eg_cq_node *next; /* point next node */
    void *buf; /* pre-allocated element buf */
    void *val; /* store element */
}eg_cq_node_t;

typedef struct eg_cg_buckets
{
    eg_cq_node_t *head; /* point a node array */
    eg_cq_node_t *node; /* point a node array */
    struct eg_cg_buckets *next; /* point next bucket */
    void *buf; /* pre-allocated element buf */
    int size; /* bucket size, use to auto stretch */
}eg_cg_bucket_t;

typedef struct eg_cycle_queue
{
    int size; /* queue size */
    int bt_index; /* btlist size */
    int elements; /* stored element num */
    int val_size; /* store element size */
    eg_cq_node_t *head; /* queue head */
    eg_cq_node_t *tail; /* queue tail */
    eg_cg_bucket_t *btlist; /* store queue bucket, use to increment size */
}eg_cycle_queue_t;

/**
 * @brief create specified size cycle queue 
 *
 * @param size cycle queue capity
 * @param val_size store element size
 * @return cycle queue pointer
 */
eg_cycle_queue_t *eg_cq_create(int size, int val_size);

/**
 * @brief destroy cycle queue 
 *
 * @param cq point cycle queue will be destroyed
 */
void eg_cq_destroy(eg_cycle_queue_t *cq);

/**
 * @brief get cycle queue size 
 *
 * @param cq eg_cycle_queue_t pointer
 * @return cycle queue size
 */
int eg_cq_capity(eg_cycle_queue_t *cq);

/**
 * @brief judge cycle queue is empty or not
 *
 * @param cq eg_cycle_queue_t pointer
 * @return empty 1, not empty 0
 */
int eg_cq_empty(eg_cycle_queue_t *cq);

/**
 * @brief insert element to tail
 *
 * @param cq eg_cycle_queue_t pointer
 * @param val insert val to cycle queue
 * @return insert result, success 1, fail 0
 */
int eg_cq_push(eg_cycle_queue_t *cq, void *val);

/**
 * @brief remove the head element
 *
 * @param cq eg_cycle_queue_t pointer
 * @return the head element
 */
void *eg_cq_pop(eg_cycle_queue_t *cq);

/**
 * @brief init eg_cq_node_t bucket
 *
 * @param arr eg_cq_node_t array
 * @param size arr size
 * @param buf pre-allocated buffer
 * @param val_size store element size
 */
void eg_cq_initbucket(eg_cq_node_t *arr, int size, unsigned char *buf, int val_size);

/**
 * @brief get current available buf
 *
 * @param cq eg_cycle_queue_t pointer
 * @return current available buf pointer
 */
void *eg_cq_get_cur_buf(eg_cycle_queue_t *cq);

/**
 * @brief increment cycle queue size
 *
 * @param cq eg_cycle_queue_t pointer
 * @return increment result, success 1, fail 0
 */
int eg_cq_inc(eg_cycle_queue_t *cq); 

/**
 * @brief decrement cycle queue size
 *
 * @param cq eg_cycle_queue_t pointer
 */
void eg_cq_dec(eg_cycle_queue_t *cq);

/**
 * @brief access the head element
 *
 * @param cq eg_cycle_queue_t pointer
 * @return the head element
 */
void *eg_cq_head(eg_cycle_queue_t *cq);
#endif
