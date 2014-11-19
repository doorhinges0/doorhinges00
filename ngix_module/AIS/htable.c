
#include <string.h>
#include <malloc.h>


#include "htable.h"

/* htable_hash - hash a string */





static unsigned htable_hash(const char *s, unsigned size)
{
    unsigned long h = 0;
    unsigned long g;

    /*
     * From the "Dragon" book by Aho, Sethi and Ullman.
     */

    while (*s) {
	h = (h << 4U) + *s++;
	if ((g = (h & 0xf0000000)) != 0) {
	    h ^= (g >> 24U);
	    h ^= g;
	}
    }
    return (h % size);
}

/* htable_link - insert element into table */

#define htable_link(table, element) { \
     HTABLE_INFO **_h = table->data + htable_hash(element->key, table->size);\
    element->prev = 0; \
    if ((element->next = *_h) != 0) \
	(*_h)->prev = element; \
    *_h = element; \
    table->used++; \
}

/* htable_size - allocate and initialize hash table */

static void htable_size(HTABLE *table, unsigned size)
{
    HTABLE_INFO **h;

    size |= 1;

    table->data = h = (HTABLE_INFO **) malloc(size * sizeof(HTABLE_INFO *));
    table->size = size;
    table->used = 0;

    while (size-- > 0)
	*h++ = 0;
}

/* htable_create - create initial hash table */

HTABLE *htable_create(int size)
{
    HTABLE *table;

    table = (HTABLE *) malloc(sizeof(HTABLE));
    htable_size(table, size < 13 ? 13 : size);
    table->seq_element = 0;
    return (table);
}

/* htable_grow - extend existing table */

static void htable_grow(HTABLE *table)
{
    HTABLE_INFO *ht;
    HTABLE_INFO *next;
    unsigned old_size = table->size;
    HTABLE_INFO **h = table->data;
    HTABLE_INFO **old_entries = h;

    htable_size(table, 2 * old_size);

    while (old_size-- > 0) {
	for (ht = *h++; ht; ht = next) {
	    next = ht->next;
	    htable_link(table, ht);
	}
    }
    free((char *) old_entries);
}

/* htable_enter - enter (key, value) pair */
extern char   *mystrdup(const char *str);

HTABLE_INFO *htable_enter(HTABLE *table, const char *key, char *value)
{
    HTABLE_INFO *ht;

    if (table->used >= table->size && table->seq_element == 0)
	htable_grow(table);
    ht = (HTABLE_INFO *) malloc(sizeof(HTABLE_INFO));
    ht->key = mystrdup(key);
    ht->value = value;
    htable_link(table, ht);
    return (ht);
}

/* htable_find - lookup value */

char   *htable_find(HTABLE *table, const char *key)
{
    HTABLE_INFO *ht;

#define	STREQ(x,y) (x == y || (x[0] == y[0] && strcmp(x,y) == 0))

    if (table)
	for (ht = table->data[htable_hash(key, table->size)]; ht; ht = ht->next)
	    if (STREQ(key, ht->key))
		return (ht->value);
    return (0);
}

/* htable_locate - lookup entry */

HTABLE_INFO *htable_locate(HTABLE *table, const char *key)
{
    HTABLE_INFO *ht;

#define	STREQ(x,y) (x == y || (x[0] == y[0] && strcmp(x,y) == 0))

    if (table)
	for (ht = table->data[htable_hash(key, table->size)]; ht; ht = ht->next)
	    if (STREQ(key, ht->key))
		return (ht);
    return (0);
}

/* htable_delete - delete one entry */

void    htable_delete(HTABLE *table, const char *key, void (*free_fn) (char *))
{
    if (table) {
	HTABLE_INFO *ht;
	HTABLE_INFO **h = table->data + htable_hash(key, table->size);

#define	STREQ(x,y) (x == y || (x[0] == y[0] && strcmp(x,y) == 0))

	for (ht = *h; ht; ht = ht->next) {
	    if (STREQ(key, ht->key)) {
		if (ht->next)
		    ht->next->prev = ht->prev;
		if (ht->prev)
		    ht->prev->next = ht->next;
		else
		    *h = ht->next;
		table->used--;
		free(ht->key);
		if (free_fn && ht->value)
		    (*free_fn) (ht->value);
		free((char *) ht);
		return;
	    }
	}
//	msg_panic("htable_delete: unknown_key: \"%s\"", key);
    }
}

/* htable_free - destroy hash table */

void    htable_free(HTABLE *table, void (*free_fn) (char *))
{
    if (table) {
	unsigned i = table->size;
	HTABLE_INFO *ht;
	HTABLE_INFO *next;
	HTABLE_INFO **h = table->data;

	while (i-- > 0) {
	    for (ht = *h++; ht; ht = next) {
		next = ht->next;
		free(ht->key);
		if (free_fn && ht->value)
		    (*free_fn) (ht->value);
		free((char *) ht);
	    }
	}
	free((char *) table->data);
	table->data = 0;
	free((char *) table);
    }
}

/* htable_walk - iterate over hash table */

void    htable_walk(HTABLE *table, void (*action) (HTABLE_INFO *, char *),
		            char *ptr) {
    if (table) {
	unsigned i = table->size;
	HTABLE_INFO **h = table->data;
	HTABLE_INFO *ht;

	while (i-- > 0)
	    for (ht = *h++; ht; ht = ht->next)
		(*action) (ht, ptr);
    }
}

/* htable_list - list all table members */

HTABLE_INFO **htable_list(HTABLE *table)
{
    HTABLE_INFO **list;
    HTABLE_INFO *member;
    int     count = 0;
    int     i;

    if (table != 0) {
	list = (HTABLE_INFO **) malloc(sizeof(*list) * (table->used + 1));
	for (i = 0; i < table->size; i++)
	    for (member = table->data[i]; member != 0; member = member->next)
		list[count++] = member;
    } else {
	list = (HTABLE_INFO **) malloc(sizeof(*list));
    }
    list[count] = 0;
    return (list);
}

/* htable_sequence - dict(3) compatibility iterator */

HTABLE_INFO *htable_sequence(HTABLE *table, int how)
{
    if (table == 0)
	return (0);

    switch (how) {
    case HTABLE_SEQ_FIRST:			/* start new sequence */
	table->seq_bucket = table->data;
	table->seq_element = table->seq_bucket[0];
	break;
    case HTABLE_SEQ_NEXT:			/* next element */
	if (table->seq_element) {
	    table->seq_element = table->seq_element->next;
	    break;
	}
	/* FALLTHROUGH */
    default:					/* terminate sequence */
	return (table->seq_element = 0);
    }

    while (table->seq_element == 0
	   && ++(table->seq_bucket) < table->data + table->size)
	table->seq_element = table->seq_bucket[0];
    return (table->seq_element);
}
