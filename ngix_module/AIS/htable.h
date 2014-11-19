#ifndef _HTABLE_H_INCLUDED_
#define _HTABLE_H_INCLUDED_


typedef struct HTABLE_INFO {
    char   *key;			/* lookup key */
    char   *value;			/* associated value */
    struct HTABLE_INFO *next;		/* colliding entry */
    struct HTABLE_INFO *prev;		/* colliding entry */
} HTABLE_INFO;

 /* Structure of one hash table. */

typedef struct HTABLE {
    int     size;			/* length of entries array */
    int     used;			/* number of entries in table */
    HTABLE_INFO **data;			/* entries array, auto-resized */
    HTABLE_INFO **seq_bucket;		/* current sequence hash bucket */
    HTABLE_INFO *seq_element;		/* current sequence element */
} HTABLE;

extern HTABLE *htable_create(int);
extern HTABLE_INFO *htable_enter(HTABLE *, const char *, char *);
extern HTABLE_INFO *htable_locate(HTABLE *, const char *);
extern char *htable_find(HTABLE *, const char *);
extern void htable_delete(HTABLE *, const char *, void (*) (char *));
extern void htable_free(HTABLE *, void (*) (char *));
extern void htable_walk(HTABLE *, void (*) (HTABLE_INFO *, char *), char *);
extern HTABLE_INFO **htable_list(HTABLE *);
extern HTABLE_INFO *htable_sequence(HTABLE *, int);

#define HTABLE_SEQ_FIRST	0
#define HTABLE_SEQ_NEXT		1
#define HTABLE_SEQ_STOP		(-1)


#endif
