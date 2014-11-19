/*
 *       Filename:  hashtable.h
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

#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#define EG_NUM_PRIMES 28

static const unsigned long eg_prime_list[EG_NUM_PRIMES] =
{
    53ul,         97ul,         193ul,       389ul,       769ul,
    1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
    49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
    1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
    50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
    1610612741ul, 3221225473ul, 4294967291ul
};

typedef struct eg_ht_bucket
{
	const void *key; /* key used to calculate hash */
    const void *val; /* stored value */
	struct eg_ht_bucket *next; /* DLList of buckets in hash collision */
	struct eg_ht_bucket *prev; /* DLList of buckets in hash collision */
	struct eg_ht_bucket *tnext; /* DLList of all the hash buckets for traversal */
	struct eg_ht_bucket *tprev; /* DLList of all the hash buckets for traversal */
	struct eg_ht_bucket **head; /* point the same hash DLList head */
}eg_ht_bucket_t;

typedef struct eg_hashtable
{
    unsigned long (*hash) (const void *val); /* calcs a hash.*/
    int (*compare) (const void *a, const void *b);  /* compares keys, == : 1 , != : 0 */
    int (*is_resize) (struct eg_hashtable *ht); /* whether should be resized */
    unsigned long (*get_newsize) (unsigned long n); /* get a new size, based on current size */
    unsigned long size; /* size of the hash list */
	unsigned long elements; /* number of values currently stored in the table */
	struct eg_ht_bucket **hlist; /* DLList store the hashbuckets by hash calculate */
	struct eg_ht_bucket *tlist; /* DLList store all hashbuckets in the table, for traversal */
}eg_hashtable_t;

typedef struct eg_ht_iterator
{
	const void *key; /* key be used to calculate hash */
	const void *val; /* stored value */
}eg_ht_iterator_t;

typedef struct eg_ht_node_pool
{
    int size;
    int cur;
    eg_ht_bucket_t *bt_arr; 
}ht_node_pool_t;

typedef struct eg_ht_map_pool
{
    int size;
    int cur;
    eg_hashtable_t **ht_arr; 
}ht_map_pool_t;

typedef int (*eg_ht_compare) (const void *a, const void *b);
typedef int (*eg_ht_is_resize) (eg_hashtable_t *ht);
typedef unsigned long (*eg_ht_get_newsize) (unsigned long n);
typedef unsigned long (*eg_ht_hash) (const void *val);

eg_ht_bucket_t *get_ht_bucket(ht_node_pool_t *pool);
eg_hashtable_t *get_ht_map(ht_map_pool_t *pool);
void init_ht_node_pool(ht_node_pool_t *pool, int size);
void reset_ht_node_pool(ht_node_pool_t *pool);
void destroy_ht_node_pool(ht_node_pool_t *pool, int size);
void init_ht_map_pool(ht_map_pool_t *pool, int size);
void reset_ht_map_pool(ht_map_pool_t *pool);
void destroy_ht_map_pool(ht_map_pool_t *pool, int size);
void init_ht_node_pool(ht_node_pool_t *pool, int size);
void destroy_ht_node_pool(ht_node_pool_t *pool, int size);
void ht_insert_bucket(eg_hashtable_t *ht, eg_ht_bucket_t *b);
void ht_erase_bucket(eg_hashtable_t *ht, eg_ht_iterator_t **iter);
void ht_remove_bucket(eg_hashtable_t *ht, const void *key);
void ht_clear_bucket(eg_hashtable_t *ht);

/**
 * @brief calculate a integer by key 
 *
 * @param key type is integer, be used to calculate hash value
 * @return a integer value
 */
unsigned long eg_hash_num (const void *key);

/**
 * @brief calculate a integer by key 
 *
 * @param key type is string, be used to calculate hash value
 * @return a integer value
 */
unsigned long eg_hash_str (const void *key);

/**
 * @brief compare key that the type can cast to integer value, for example : int, long, pointer
 *
 * @param key1 one of the compare keys
 * @param key2 one of the compare keys
 * @return compare result, key1 == key2 : 1, key1 != key2 : 0
 */
int eg_num_cmp (const void *key1, const void *key2);

/**
 * @brief compare key that the type is string, and case sensitive
 *
 * @param key1 one of the compare keys
 * @param key2 one of the compare keys
 * @return compare result, key1 == key2 : 1, key1 != key2 : 0
 */
int eg_str_cpm (const void *key1, const void *key2);

/**
 * @brief compare key that the type is string, and not case sensitive
 *
 * @param key1 one of the compare keys
 * @param key2 one of the compare keys
 * @return compare result, key1 == key2 : 1, key1 != key2 : 0
 */
int eg_str_case_cpm (const void *key1, const void *key2);

/**
 * @brief calcuate a new size, base on current size
 *
 * @param n current hashtable size
 * @return the new hashtable size
 */
unsigned long eg_next_prime(unsigned long n);

/**
 * @brief check size whether need resize
 *
 * @param ht the check hashtable
 * @return check result, resize : 1, not resize : 0
 */
int eg_is_resize(eg_hashtable_t *ht);

/**
 * @brief create new hashtable by some param
 *
 * @param size the hashtable capacity
 * @param h the hash function, if NULL, the key is hash
 * @param c the key compare function, if NULL, compare key as integer
 * @param r whether shoule be reszie function, if NULL, use default
 * @param n get new capacity function, is NULL, use default
 * @param mpl the memory pool, if NULL, use system func
 * @return the new hashtable pointer
 */
eg_hashtable_t *eg_ht_create(unsigned long size, eg_ht_hash h, eg_ht_compare c,
                             eg_ht_is_resize r, eg_ht_get_newsize n);
/**
 * @brief destroy the hashtable 
 *
 * @param ht the hashtable will be destroyed
 */
void eg_ht_destroy(eg_hashtable_t *ht);
/**
 * @brief insert key-val
 *
 * @param ht the hashtable val insert to it
 * @param key be used to calculate hash
 * @param val be stored to hashtable
 * @param mpl the memory pool, if NULL, use system func
 * @return insert result, success : 1, failed : 0
 */
int eg_ht_insert(eg_hashtable_t *ht, const void *key, const void *val);

/**
 * @brief remove val by key
 *
 * @param ht the hashtable val remove from it
 * @param key be used to calculate hash
 * @param mpl the memory pool, if NULL, use system func
 * @return remove result, success : 1, failed : 0
 */
int eg_ht_remove(eg_hashtable_t *ht, const void *key);

/**
 * @brief resize the hashtable
 *
 * @param ht the hashtable will be resized
 * @param newsize the hashtable size after resize
 * @param mpl the memory pool, if NULL, use system func
 * @return remove result, success : 1, failed : 0
 */
int eg_ht_resize(eg_hashtable_t *ht, unsigned long newsize);

/**
 * @brief remove val by iterator
 *
 * @param iter the iterator pointer val
 * @param mpl the memory pool, if NULL, use system func
 */
int eg_ht_erase(eg_hashtable_t *ht, eg_ht_iterator_t **iter);

/**
 * @brief clear all values
 *
 * @param ht the hashtable vals remove from it
 * @param mpl the memory pool, if NULL, use system func
 */
void eg_ht_clear(eg_hashtable_t *ht);

/**
 * @brief find iterator by key
 *
 * @param ht the hashtable val find from it
 * @param key be used to calculate hash
 * @return the found iterator
 */
eg_ht_iterator_t *eg_ht_find(eg_hashtable_t *ht, const void *key);

/**
 * @brief get the head pointer for traversal 
 *
 * @param ht the hashtable will be traversaled
 * @return the iterator point the head
 */
eg_ht_iterator_t *eg_ht_begin(eg_hashtable_t *ht);

/**
 * @brief get the end pointer for traversal 
 *
 * @param ht the hashtable will be traversaled
 * @return the iterator point the end
 */
eg_ht_iterator_t *eg_ht_end();

/**
 * @brief get the next iterator 
 *
 * @param it current iterator
 * @return the next iterator
 */
void eg_ht_next(eg_ht_iterator_t **it);

/**
 * @brief get the hashtable capacity 
 *
 * @param ht a hashtable
 * @return the hashtable capacity
 */
unsigned long eg_ht_capacity(eg_hashtable_t *ht);

/**
 * @brief get the hashtable stored elements 
 *
 * @param ht a hashtable
 * @return the hashtable stored elements
 */
unsigned long eg_ht_elements(eg_hashtable_t *ht);

/**
 * @brief judge the hashtable is empty or not 
 *
 * @param ht a hashtable
 * @return the judge result, empty :1, not : 0
 */
int eg_ht_empty(eg_hashtable_t *ht);

#endif
