/*
 *       Filename:  hashtable.c
 *
 *    Description:  don't use double as key or value, because hash don't support it.
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan Chen, ivanchen36@gmail.com
 *   Organization:  
 */
#include <string.h>
#include <stdlib.h>
#include "hashtable.h"

void reset_ht_node_pool(ht_node_pool_t *pool)
{
    pool->cur = 0;
}

void reset_ht_map_pool(ht_map_pool_t *pool)
{
    pool->cur = 0;
}

eg_ht_bucket_t *get_ht_bucket(ht_node_pool_t *pool)
{
    if(pool->cur >= pool->size) return NULL;

    return &pool->bt_arr[pool->cur++]; 
}

eg_hashtable_t *get_ht_map(ht_map_pool_t *pool)
{
    if(pool->cur >= pool->size) return NULL;

    return pool->ht_arr[pool->cur++]; 
}

void init_ht_node_pool(ht_node_pool_t *pool, int size)
{
    pool->bt_arr = malloc(sizeof(eg_ht_bucket_t) * size);
    pool->cur = 0;
    pool->size = pool->bt_arr ? size : 0;
}

void destroy_ht_node_pool(ht_node_pool_t *pool, int size)
{
    if(pool->bt_arr) free(pool->bt_arr);
    pool->cur = 0;
    pool->size = 0;
}

void init_ht_map_pool(ht_map_pool_t *pool, int size)
{
    pool->ht_arr = malloc(sizeof(eg_hashtable_t *) * size);
    pool->cur = 0;
    pool->size = pool->ht_arr ? size : 0;
}

void destroy_ht_map_pool(ht_map_pool_t *pool, int size)
{
    if(pool->ht_arr) free(pool->ht_arr);
    pool->cur = 0;
    pool->size = 0;
}

void ht_insert_bucket(eg_hashtable_t *ht, eg_ht_bucket_t *b)
{
	if (!ht || !b) return;

    unsigned long h;
    eg_ht_bucket_t *bt;

    h = (*ht->hash)(b->key);
    h %= ht->size;
    for (bt = ht->hlist[h]; bt; bt = bt->next)
    {
        if ((*ht->compare)(b->key, bt->key)) return;
    }

    b->head = &ht->hlist[h];
    b->next = ht->hlist[h];
    b->prev = NULL;
    b->tnext = ht->tlist;
    b->tprev = NULL;
    ht->hlist[h] = b;
    ht->tlist = b;
    if (b->next)
    {
        b->next->prev = b;
    }
    if (b->tnext)
    {
        b->tnext->tprev = b;   
    }
    ++ht->elements;
}

void ht_erase_bucket(eg_hashtable_t *ht, eg_ht_iterator_t **iter)
{
    if (!ht || !*iter) return;

    eg_ht_bucket_t *b = (eg_ht_bucket_t *)*iter;
    *iter = (eg_ht_iterator_t *)b->tnext;
    if (b->prev && b->next)
    {
        b->prev->next = b->next;
        b->next->prev = b->prev;
    }else if (b->prev)
    {
        b->prev->next = b->next;
    }else if (b->next)
    {
        *b->head = b->next;
        b->next->prev = b->prev;
    }else
    {
        *b->head = NULL;
    }

    if (b->tprev && b->tnext)
    {
        b->tprev->tnext = b->tnext;
        b->tnext->tprev = b->tprev;
    }else if (b->tprev)
    {
        b->tprev->tnext = b->tnext;
    }else if (b->tnext)
    {
        ht->tlist = b->tnext;
        b->tnext->tprev = b->tprev;
    }else
    {
        ht->tlist = NULL;
    }
    --ht->elements;
}

void ht_remove_bucket(eg_hashtable_t *ht, const void *key)
{
    if (!ht || !key) return;
    
    eg_ht_iterator_t *it = eg_ht_find(ht, key);

    if(!it) return;

    ht_erase_bucket(ht, &it);
}

void ht_clear_bucket(eg_hashtable_t *ht)
{
    memset(ht->hlist, 0, ht->size * sizeof(eg_ht_bucket_t *));
    ht->tlist = NULL;
    ht->elements = 0;
}

unsigned long eg_hash_num (const void *key)
{
    return (unsigned long)key;
}

unsigned long eg_hash_str (const void *key)
{
    const char *str = (const char *)key;
    unsigned long sum = 0;;

    for ( ; *str; ++str) sum = 5 * sum + *str;

    return sum;
}

int eg_num_cmp (const void *key1, const void *key2)
{
    return key1 == key2;
}

int eg_str_cpm (const void *key1, const void *key2)
{
    return strcmp((const char*)key1, (const char*)key2) ? 0 : 1;
}

int eg_str_case_cpm (const void *key1, const void *key2)
{
    return strcasecmp((const char*)key1, (const char*)key2) ? 0 : 1;
}

unsigned long eg_next_prime(unsigned long n)
{
    int len = EG_NUM_PRIMES - 1;
    int half = len >> 1;
    const unsigned long *f = eg_prime_list;
    const unsigned long *m;

    while (len > 0)
    {
        half = len >> 1;
        m = f + half;
        if (*m < n)
        {
            f = m;
            ++f;
            len = len - half - 1;
        }
        else
        {
            len = half;
        }
    }

    return *f;   
}

int eg_is_resize(eg_hashtable_t *ht)
{
    return ht->size < ht->elements;
}

eg_hashtable_t *eg_ht_create(unsigned long size, eg_ht_hash h, eg_ht_compare c,
        eg_ht_is_resize r, eg_ht_get_newsize n)
{
    eg_hashtable_t *ht;
    unsigned long len = sizeof(eg_hashtable_t);

    ht = (eg_hashtable_t *)malloc(len);
    if (!ht) return NULL;

    memset(ht, 0, len);
    size = eg_next_prime(size);
    len = size * sizeof(eg_ht_bucket_t *);
    ht->hlist = (eg_ht_bucket_t **)malloc(len);

    if (!ht->hlist)
    {
        free(ht);

        return NULL;
    }

    memset(ht->hlist, 0, len);
    ht->size = size;
    if (c) ht->compare = c; else ht->compare = eg_num_cmp;
    if (r) ht->is_resize = r; else ht->is_resize = eg_is_resize;
    if (n) ht->get_newsize = n; else ht->get_newsize = eg_next_prime;
    if (h) ht->hash = h; else ht->hash = eg_hash_num;

    return ht;
}

void eg_ht_destroy(eg_hashtable_t *ht)
{
    if (!ht) return;

    eg_ht_clear(ht);
    free(ht->hlist);
    free(ht);
    ht = NULL;
}

int eg_ht_insert(eg_hashtable_t *ht, const void *key, const void *val)
{
    if (!ht || !key) return 0;

    unsigned long h;
    eg_ht_bucket_t *b;

    h = (*ht->hash)(key);
    h %= ht->size;
    for (b = ht->hlist[h]; b; b = b->next)
    {
        if ((*ht->compare)(key, b->key)) return 0;
    }

    b = (eg_ht_bucket_t *)malloc(sizeof(eg_ht_bucket_t));
    if (!b) return 0;

    if ((*ht->is_resize)(ht))
    {
        eg_ht_resize(ht, (*ht->get_newsize)(ht->size));
    }
    b->key = key;
    b->val = val;
    b->head = &ht->hlist[h];
    b->next = ht->hlist[h];
    b->prev = NULL;
    b->tnext = ht->tlist;
    b->tprev = NULL;
    ht->hlist[h] = b;
    ht->tlist = b;
    if (b->next)
    {
        b->next->prev = b;
    }
    if (b->tnext)
    {
        b->tnext->tprev = b;   
    }
    ++ht->elements;

    return 1;
}

int eg_ht_resize(eg_hashtable_t *ht, unsigned long newsize)
{
    unsigned long h;
    eg_ht_bucket_t *b;
    unsigned long len = newsize * sizeof(eg_ht_bucket_t *);

    free(ht->hlist);
    ht->hlist = (eg_ht_bucket_t **)malloc(len);
    memset(ht->hlist, 0, len);
    ht->size = newsize;

    for(b = ht->tlist; b; b = b->tnext)
    {
        h = (*ht->hash)(b->key);
        h %= newsize;
        b->head = &ht->hlist[h];
        b->next = ht->hlist[h];
        b->prev = NULL;
        ht->hlist[h] = b;
        if (b->next)
        {
            b->next->prev = b;
        }
    }

    return 1;
}

int eg_ht_remove(eg_hashtable_t *ht, const void *key)
{
    if (!ht || !key) return 0;

    eg_ht_iterator_t *it = eg_ht_find(ht, key);

    if(!it) return 0;
    
    return eg_ht_erase(ht, &it); 
}

void eg_ht_clear(eg_hashtable_t *ht)
{
    if(!ht || !ht->elements) return;

    eg_ht_bucket_t *b;
    eg_ht_bucket_t *b1;

    memset(ht->hlist, 0, ht->size * sizeof(eg_ht_bucket_t *));
    ht->elements = 0;
    for (b = ht->tlist; b; ) 
    {
        b1 = b;
        b = b->tnext;
        free(b1);
    }
    ht->tlist = NULL;
}

eg_ht_iterator_t *eg_ht_find(eg_hashtable_t *ht, const void *key)
{
    if (!ht || !key) return NULL;

    eg_ht_bucket_t *b;
    unsigned long h = (*ht->hash)(key);

    h %= ht->size;
    for (b = ht->hlist[h]; b; b = b->next)
    {
        if ((*ht->compare)(key, b->key)) break;
    }

    return (eg_ht_iterator_t *)b;
}

int eg_ht_erase(eg_hashtable_t *ht, eg_ht_iterator_t **iter)
{
    if (!ht || !*iter) return 0;

    eg_ht_bucket_t *b = (eg_ht_bucket_t *)*iter;
    *iter = (eg_ht_iterator_t *)b->tnext;
    if (b->prev && b->next)
    {
        b->prev->next = b->next;
        b->next->prev = b->prev;
    }else if (b->prev)
    {
        b->prev->next = b->next;
    }else if (b->next)
    {
        *b->head = b->next;
        b->next->prev = b->prev;
    }else
    {
        *b->head = NULL;
    }

    if (b->tprev && b->tnext)
    {
        b->tprev->tnext = b->tnext;
        b->tnext->tprev = b->tprev;
    }else if (b->tprev)
    {
        b->tprev->tnext = b->tnext;
    }else if (b->tnext)
    {
        ht->tlist = b->tnext;
        b->tnext->tprev = b->tprev;
    }else
    {
        ht->tlist = NULL;
    }

    free(b);
    b = NULL;
    --ht->elements;

    return 1;
}

eg_ht_iterator_t *eg_ht_begin(eg_hashtable_t *ht)
{
    if (!ht) return NULL;

    return (eg_ht_iterator_t *)ht->tlist;
}

eg_ht_iterator_t *eg_ht_end()
{
    return NULL;
}

void eg_ht_next(eg_ht_iterator_t **it)
{
    eg_ht_bucket_t* bt = (eg_ht_bucket_t*)(*it);
    *it = (eg_ht_iterator_t*)bt->tnext;
}

unsigned long eg_ht_capacity(eg_hashtable_t *ht)
{
    return ht->size;
}

unsigned long eg_ht_elements(eg_hashtable_t *ht)
{
    return ht->elements;
}

inline int eg_ht_empty(eg_hashtable_t *ht)
{
    return 0 == ht->elements;
}
