/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 *  ais - memory caching daemon
 *
 *       http://www.danga.com/ais/
 *
 *  Copyright 2003 Danga Interactive, Inc.  All rights reserved.
 *
 *  Use and distribution licensed under the BSD license.  See
 *  the LICENSE file for full text.
 *
 *  Authors:
 *      Anatoly Vorobey <mellon@pobox.com>
 *      Brad Fitzpatrick <brad@danga.com>
 */

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <ctype.h>
#include <stdarg.h>
/* some POSIX systems need the following definition
 * to get mlockall flags out of sys/mman.h.  */
#ifndef _P1003_1B_VISIBLE
#define _P1003_1B_VISIBLE
#endif
/* need this to get IOV_MAX on some platforms. */
#ifndef __need_IOV_MAX
#define __need_IOV_MAX
#endif
#include <pwd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <limits.h>
#include <sysexits.h>
#include <stddef.h>
#include "def.h"
#include "ais.h"


/* FreeBSD 4.x doesn't have IOV_MAX exposed. */
#ifndef IOV_MAX
#if defined(__FreeBSD__) || defined(__APPLE__)
#define IOV_MAX 1024
#endif
#endif

void out_string(conn *c, const char *str);

#define IFELSE(e1,e2) ((e1) ? (e1) : (e2))

#define GETTIMEOFDAY(t)	gettimeofday(t,(struct timezone *) 0)
#ifdef DEBUG_TIME	

static int64_t getmocrtime()
{
	 struct timeval t;
	GETTIMEOFDAY(&t);

	return t.tv_sec*1000000+t.tv_usec ;
}
#endif

#ifdef DEBUG

#define DEBUG_INFO(x) do { \
		flockfile(stdout); \
        struct tm  t;\
    	time_t second = time(NULL);\
    	localtime_r(&second, &t);\
    	t.tm_mon++;\
    	t.tm_year += 1900;\
        char err_time[64];\
        sprintf(err_time, "%4d/%02d/%02d %02d:%02d:%02d", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour,t.tm_min, t.tm_sec); \
        printf("%s.%lu.%s.%d: ", \
        err_time, pthread_self(),  __func__, __LINE__); \
        printf x; \
        putchar('\n'); \
        fflush(stdout); \
        funlockfile(stdout);\
} while (0)
#else
#define DEBUG_INFO(x)
#endif


#include "allais.c"

#ifdef TXT_LOG
FILE *record_fp = NULL;
FILE *recordcallback_fp = NULL;
static pthread_t maintenance_tid;
static volatile int do_run_maintenance_thread = 1;
#endif
/*
 * forward declarations
 */
static void drive_machine(conn *c);
static int new_socket(struct addrinfo *ai);
static int try_read_command(conn *c);

enum try_read_result {
    READ_DATA_RECEIVED,
    READ_NO_DATA_RECEIVED,
    READ_ERROR,            /** an error occured (on the socket) (or client closed connection) */
    READ_MEMORY_ERROR      /** failed to allocate more memory */
};

static enum try_read_result try_read_network(conn *c);
static enum try_read_result try_read_udp(conn *c);

static void conn_set_state(conn *c, enum conn_states state);

/* stats */
static void stats_init(void);
static void server_stats(ADD_STAT add_stats, conn *c);
static void process_stat_settings(ADD_STAT add_stats, void *c);


/* defaults */
static void settings_init(void);

/* event handling, network IO */
static void event_handler(const int fd, const short which, void *arg);
static bool update_event(conn *c, const int new_flags);
void process_command(conn *c, char *command);
static void write_and_free(conn *c, char *buf, int bytes);
static int ensure_iov_space(conn *c);
static int add_iov(conn *c, const void *buf, int len);
static int add_msghdr(conn *c);

/** exported globals **/
struct stats stats;
struct settings settings;
struct ais_context  g_ctx;

/* add by chenhuizhong */
sql_conn_t *g_sql_conn;
struct redisContext **g_redis_conn;
struct redisContext **g_rredis_conn;
struct redisContext **g_sredis_conn;
struct redisContext *g_log_redis_conn;
ht_node_pool_t *g_ht_node_pools;
ht_map_pool_t *g_ht_map_pools;
cache_pool_t **g_conn_pools;
eg_cycle_queue_t **g_conn_item_queues;
#ifdef TXT_LOG
eg_cycle_queue_t **g_log_queues;
int g_log_empty;
sem_t g_log_sem;
#endif

time_t process_started;     /* when the process was started */

/** file scope variables **/
static conn *listen_conn = NULL;
static struct event_base *main_base;

enum transmit_result {
    TRANSMIT_COMPLETE,   /** All done writing. */
    TRANSMIT_INCOMPLETE, /** More data remaining to write. */
    TRANSMIT_SOFT_ERROR, /** Can't write any more right now. */
    TRANSMIT_HARD_ERROR  /** Can't write (c->state is set to conn_closing) */
};

static enum transmit_result transmit(conn *c);

/* This reduces the latency without adding lots of extra wiring to be able to
 * notify the listener thread of when to listen again.
 * Also, the clock timer could be broken out into its own thread and we
 * can block the listener via a condition.
 */
static volatile bool allow_new_conns = true;
static struct event maxconnsevent;
static void maxconns_handler(const int fd, const short which, void *arg) {
    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = 10000;

    if (fd == -42 || allow_new_conns == false) {
        /* reschedule in 10ms if we need to keep polling */
        evtimer_set(&maxconnsevent, maxconns_handler, 0);
        event_base_set(main_base, &maxconnsevent);
        evtimer_add(&maxconnsevent, &t);
    } else {
        evtimer_del(&maxconnsevent);
        accept_new_conns(true);
    }
}

#define REALTIME_MAXDELTA 60*60*24*30
 
static void stats_init(void) {
    stats.curr_items = stats.total_items = stats.curr_conns = stats.total_conns = stats.conn_structs = 0;
    stats.get_cmds = stats.set_cmds = stats.get_hits = stats.get_misses = stats.evictions = stats.reclaimed = 0;
    stats.touch_cmds = stats.touch_misses = stats.touch_hits = stats.rejected_conns = 0;
    stats.curr_bytes = stats.listen_disabled_num = 0;
    stats.hash_power_level = stats.hash_bytes = stats.hash_is_expanding = 0;
    stats.expired_unfetched = stats.evicted_unfetched = 0;
    stats.accepting_conns = true; /* assuming we start in this state. */

    /* make the time we started always be 2 seconds before we really
       did, so time(0) - time.started is never zero.  if so, things
       like 'settings.oldest_live' which act as booleans as well as
       values are now false in boolean context... */
    process_started = time(0) - 2;
}
#define MAX_OPTIONS 100
#define MAX_CONF_FILE_LINE_SIZE (8 * 1024)
#define CONFIG_FILE "ais.conf"

static void die(const char *fmt, ...) {
  va_list ap;
  char msg[200];

  va_start(ap, fmt);
  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);

  fprintf(stderr, "%s\n", msg);

  exit(EXIT_FAILURE);
}

static char *sdup(const char *str) {
  char *p;
  if ((p = (char *) malloc(strlen(str) + 1)) != NULL) {
    strcpy(p, str);
  }
  return p;
}

char config_file[PATH_MAX];  // Set by process_command_line_arguments()
//char	tagchinautf8[512];

static void setting_init2(void){

    //	gb2312_to_utf8( tag_address_country , tagchinautf8 );
    char *options[MAX_OPTIONS] = {0,};
    FILE *fp = NULL;
    char line[MAX_CONF_FILE_LINE_SIZE], opt[sizeof(line)], val[sizeof(line)];
    int  i,  line_no = 0;
    int opt_len = -1;
    const char *default_value;
    if(config_file[0] == 0)
        snprintf(config_file, sizeof(config_file), "./%s", CONFIG_FILE);

    fp = fopen(config_file, "r");

    // If config file was set in command line and open failed, die
    if (fp == NULL) {
        die("Cannot open config file %s: %s", config_file, strerror(errno));
    }

    // Load config file settings first
    if (fp != NULL) {
        fprintf(stderr, "Loading config file %s\n", config_file);

        // Loop over the lines in config file
        while (fgets(line, sizeof(line), fp) != NULL) {

            line_no++;

            // Ignore empty lines and comments
            if (line[0] == '#' || line[0] == '\n' ||line[0] == '/')
                continue;

            if (sscanf(line, "%s %[^\r\n#]", opt, val) != 2) {
                die("%s: line %d is invalid", config_file, (int) line_no);
            }

            options[++opt_len] = sdup(opt);
            options[++opt_len] = sdup(val);
            if (opt_len == MAX_OPTIONS - 3) {
                die("%s", "Too many options specified");
            }
        }

        (void) fclose(fp);
    }

    if(opt_len > -1)
    {
        for(; opt_len != -1; )
        {
            if ((i = get_option_index(options[opt_len - 1])) == -1) {
                die("Invalid option: %s", options[opt_len - 1]);
                //cry(fc(ctx), "Invalid option: %s", name);
                //free_context(ctx);
                return ;
            } else if (!options[opt_len]) {
                die("%s: option value cannot be NULL", options[opt_len - 1]);
                //    cry(fc(ctx), "%s: option value cannot be NULL", name);
                //  free_context(ctx);
                return ;
            }
            if (g_ctx.config[i] != NULL) {
                die("warning: %s: duplicate option", options[opt_len - 1]);
                //cry(fc(ctx), "warning: %s: duplicate option", name);
            }
            g_ctx.config[i] = ais_strdup(options[opt_len]);
            free(options[opt_len--]);
            free(options[opt_len--]);
            //DEBUG_TRACE(("[%s] -> [%s]", name, value));
        }
    }
    for ( i = 0; config_options[i * ENTRIES_PER_CONFIG_OPTION] != NULL; i++)
    {
        default_value = config_options[i * ENTRIES_PER_CONFIG_OPTION + 2];
        if (g_ctx.config[i] == NULL && default_value != NULL) {
            g_ctx.config[i] = ais_strdup(default_value);
            DEBUG_TRACE(("Setting default: [%s] -> [%s]",
                        config_options[i * ENTRIES_PER_CONFIG_OPTION + 1],
                        default_value));
        }
    }
    settings.port = atoi( g_ctx.config[LISTENING_PORTS]);
    settings.num_threads = atoi( g_ctx.config[NUM_THREADS]);

    g_ctx.img_size_len = -1;
    if(g_ctx.config[IMG_SIZE])
    {
        char *s1 = g_ctx.config[IMG_SIZE];
        char *s2 = NULL;

        while((s2 = strsep(&s1, ",")))
        {
            g_ctx.img_size[++g_ctx.img_size_len] = s2;
        }
    }
    ++g_ctx.img_size_len;
}

static void settings_init(void) {
    settings.use_cas = true;
    settings.access = 0777;
    settings.udpport = 0;
    /* By default this string should be NULL for getaddrinfo() */
    settings.inter = NULL;
    settings.maxconns = 65535;         /* to limit connections-related memory to about 5MB */
    settings.verbose = 0;
    settings.oldest_live = 0;
    settings.socketpath = NULL;       /* by default, not using a unix socket */
    settings.num_threads_per_udp = 0;
    settings.detail_enabled = 0;
    settings.reqs_per_event = 20;
    settings.backlog = 10240;
    settings.binding_protocol = negotiating_prot;
    settings.maxconns_fast = false;
    settings.hashpower_init = 0;
    strcpy(settings.conn_str,"SQL2005");
	strcpy(settings.sql_user_name,"maspsa");
	strcpy(settings.sql_user_pwd,"masp.123456");
	strcpy(settings.db_name,"MASP");
}

static void settings_destroy()
{
    int i;

    for(i = 0; i < NUM_OPTIONS; ++i)
    {
        if(g_ctx.config[i]) free(g_ctx.config[i]);
    }
}

/*
 * Adds a message header to a connection.
 *
 * Returns 0 on success, -1 on out-of-memory.
 */
static int add_msghdr(conn *c)
{
    struct msghdr *msg;

    assert(c != NULL);

    if (c->msgsize == c->msgused) {
        msg = (msghdr *)realloc(c->msglist, c->msgsize * 2 * sizeof(struct msghdr));
        if (! msg)
            return -1;
        c->msglist = msg;
        c->msgsize *= 2;
    }

    msg = c->msglist + c->msgused;

    /* this wipes msg_iovlen, msg_control, msg_controllen, and
       msg_flags, the last 3 of which aren't defined on solaris: */
    memset(msg, 0, sizeof(struct msghdr));

    msg->msg_iov = &c->iov[c->iovused];

    if (c->request_addr_size > 0) {
        msg->msg_name = &c->request_addr;
        msg->msg_namelen = c->request_addr_size;
    }

    c->msgbytes = 0;
    c->msgused++;

    if (IS_UDP(c->transport)) {
        /* Leave room for the UDP header, which we'll fill in later. */
        return add_iov(c, NULL, UDP_HEADER_SIZE);
    }

    return 0;
}

static const char *prot_text(enum protocol prot) {
    char *rv = "unknown";
    switch(prot) {
        case ascii_prot:
            rv = "ascii";
            break;
        case binary_prot:
            rv = "binary";
            break;
        case negotiating_prot:
            rv = "auto-negotiate";
            break;
    }
    return rv;
}

//modify for debug time
conn *conn_new(const int sfd, enum conn_states init_state,
                const int event_flags,
                const int read_buffer_size, enum network_transport transport,
                struct event_base *base, int64_t mocrtime, int tid) 
{
    struct ais_connection *ac;
    conn *c = (conn *)cp_pop(g_conn_pools[tid]);
    if(!c) return NULL;

    c->tid = tid;
    c->transport = transport;
    c->protocol = settings.binding_protocol;

    /* unix socket mode doesn't need this, so zeroed out.  but why
     * is this done for every command?  presumably for UDP
     * mode.  */
    if (!settings.socketpath) {
        c->request_addr_size = sizeof(c->request_addr);
    } else {
        c->request_addr_size = 0;
    }

    if (settings.verbose > 1) {
        if (init_state == conn_listening) {
            fprintf(stderr, "<%d server listening (%s)\n", sfd,
                prot_text(c->protocol));
        } else if (IS_UDP(transport)) {
            fprintf(stderr, "<%d server listening (udp)\n", sfd);
        } else if (c->protocol == negotiating_prot) {
            fprintf(stderr, "<%d new auto-negotiating client connection\n",
                    sfd);
        } else if (c->protocol == ascii_prot) {
            fprintf(stderr, "<%d new ascii client connection.\n", sfd);
        } else if (c->protocol == binary_prot) {
            fprintf(stderr, "<%d new binary client connection.\n", sfd);
        } else {
            fprintf(stderr, "<%d new unknown (%d) client connection\n",
                sfd, c->protocol);
            assert(false);
        }
    }
	ac = (ais_connection*)c->bulk;
	ac->buf_size = MAX_REQUEST_BUF_SIZE;
	ac->adparams_buf_last = ac->adparams_buf = (char *) (ac + 1);
	
	struct ais_adparams *ap = &(ac->ap);
	struct ais_adparams_callback *apc = &(ac->apc);
	memset(ap,0,sizeof(*ap));
	memset(apc,0,sizeof(*apc));
#ifdef DEBUG_TIME	

	ac->org_start = mocrtime; //add for debug time
#endif

    c->sfd = sfd;
    c->state = init_state;
    c->rlbytes = 0;
    c->rbytes = c->wbytes = 0;
    c->wcurr = c->wbuf;
    c->rcurr = c->rbuf;
    c->ritem = 0;
    c->iovused = 0;
    c->msgcurr = 0;
    c->msgused = 0;

    c->write_and_go = init_state;
    c->write_and_free = 0;
    c->noreply = false;

    event_set(&c->event, sfd, event_flags, event_handler, (void *)c);
    event_base_set(base, &c->event);
    c->ev_flags = event_flags;

    if (event_add(&c->event, 0) == -1) {
        cp_push(g_conn_pools[c->tid], c);

        perror("event_add");
        return NULL;
    }

    //STATS_LOCK();
    //stats.curr_conns++;
    //stats.total_conns++;
    //STATS_UNLOCK();

    return c;
}

static void conn_cleanup(conn *c) {
    assert(c != NULL);
#if 0// DEL_CONN_NREAD

    if (c->item) {
        item_remove(c->item);
        c->item = 0;
    }
#endif


    if (c->write_and_free) {
        free(c->write_and_free);
        c->write_and_free = 0;
    }


    if (IS_UDP(c->transport)) {
        conn_set_state(c, conn_read);
    }

	
	
}

static void conn_close(conn *c) {
    assert(c != NULL);

    /* delete the event, the socket and the conn */
    event_del(&c->event);

    if (settings.verbose > 1)
        fprintf(stderr, "<%d connection closed.\n", c->sfd);

    close(c->sfd);
    allow_new_conns = true;
    conn_cleanup(c);
    cp_push(g_conn_pools[c->tid], c);

    return;
}

/*
 * Shrinks a connection's buffers if they're too big.  This prevents
 * periodic large "get" requests from permanently chewing lots of server
 * memory.
 *
 * This should only be called in between requests since it can wipe output
 * buffers!
 */
#if 0
static void conn_shrink(conn *c) {
    assert(c != NULL);

    if (IS_UDP(c->transport))
        return;

    if (c->rsize > READ_BUFFER_HIGHWAT && c->rbytes < DATA_BUFFER_SIZE) {
        char *newbuf;

        if (c->rcurr != c->rbuf)
            memmove(c->rbuf, c->rcurr, (size_t)c->rbytes);

        newbuf = (char *)realloc((void *)c->rbuf, DATA_BUFFER_SIZE);

        if (newbuf) {
            c->rbuf = newbuf;
            c->rsize = DATA_BUFFER_SIZE;
        }
        /* TODO check other branch... */
        c->rcurr = c->rbuf;
    }


    if (c->msgsize > MSG_LIST_HIGHWAT) {
        struct msghdr *newbuf = (struct msghdr *) realloc((void *)c->msglist, MSG_LIST_INITIAL * sizeof(c->msglist[0]));
        if (newbuf) {
            c->msglist = newbuf;
            c->msgsize = MSG_LIST_INITIAL;
        }
    /* TODO check error condition? */
    }

    if (c->iovsize > IOV_LIST_HIGHWAT) {
        struct iovec *newbuf = (struct iovec *) realloc((void *)c->iov, IOV_LIST_INITIAL * sizeof(c->iov[0]));
        if (newbuf) {
            c->iov = newbuf;
            c->iovsize = IOV_LIST_INITIAL;
        }
    /* TODO check return value */
    }
}
#endif

/**
 * Convert a state name to a human readable form.
 */
static const char *state_text(enum conn_states state) {
    const char* const statenames[] = { "conn_listening",
                                       "conn_new_cmd",
                                       "conn_waiting",
                                       "conn_read",
                                       "conn_parse_cmd",
                                       "conn_write",
                                       
                                       "conn_swallow",
                                       "conn_closing",
                                       "conn_mwrite" };
    return statenames[state];
}

/*
 * Sets a connection's current state in the state machine. Any special
 * processing that needs to happen on certain state transitions can
 * happen here.
 */
static void conn_set_state(conn *c, enum conn_states state) {
    assert(c != NULL);
    assert(state >= conn_listening && state < conn_max_state);

    if (state != c->state) {
        if (settings.verbose > 2) {
            fprintf(stderr, "%d: going from %s to %s\n",
                    c->sfd, state_text(c->state),
                    state_text(state));
        }

   
        c->state = state;
    }
}

/*
 * Ensures that there is room for another struct iovec in a connection's
 * iov list.
 *
 * Returns 0 on success, -1 on out-of-memory.
 */
static int ensure_iov_space(conn *c) {
    assert(c != NULL);

    if (c->iovused >= c->iovsize) {
        int i, iovnum;
        struct iovec *new_iov = (struct iovec *)realloc(c->iov,
                                (c->iovsize * 2) * sizeof(struct iovec));
        if (! new_iov)
            return -1;
        c->iov = new_iov;
        c->iovsize *= 2;

        /* Point all the msghdr structures at the new list. */
        for (i = 0, iovnum = 0; i < c->msgused; i++) {
            c->msglist[i].msg_iov = &c->iov[iovnum];
            iovnum += c->msglist[i].msg_iovlen;
        }
    }

    return 0;
}


/*
 * Adds data to the list of pending data that will be written out to a
 * connection.
 *
 * Returns 0 on success, -1 on out-of-memory.
 */

static int add_iov(conn *c, const void *buf, int len) {
    struct msghdr *m;
    int leftover;
    bool limit_to_mtu;

    assert(c != NULL);

    do {
        m = &c->msglist[c->msgused - 1];

        /*
         * Limit UDP packets, and the first payloads of TCP replies, to
         * UDP_MAX_PAYLOAD_SIZE bytes.
         */
        limit_to_mtu = IS_UDP(c->transport) || (1 == c->msgused);

        /* We may need to start a new msghdr if this one is full. */
        if (m->msg_iovlen == IOV_MAX ||
            (limit_to_mtu && c->msgbytes >= UDP_MAX_PAYLOAD_SIZE)) {
            add_msghdr(c);
            m = &c->msglist[c->msgused - 1];
        }

        if (ensure_iov_space(c) != 0)
            return -1;

        /* If the fragment is too big to fit in the datagram, split it up */
        if (limit_to_mtu && len + c->msgbytes > UDP_MAX_PAYLOAD_SIZE) {
            leftover = len + c->msgbytes - UDP_MAX_PAYLOAD_SIZE;
            len -= leftover;
        } else {
            leftover = 0;
        }

        m = &c->msglist[c->msgused - 1];
        m->msg_iov[m->msg_iovlen].iov_base = (void *)buf;
        m->msg_iov[m->msg_iovlen].iov_len = len;

        c->msgbytes += len;
        c->iovused++;
        m->msg_iovlen++;

        buf = ((char *)buf) + len;
        len = leftover;
    } while (leftover > 0);

    return 0;
}


/*
 * Constructs a set of UDP headers and attaches them to the outgoing messages.
 */
static int build_udp_headers(conn *c) {
    int i;
    unsigned char *hdr;

    assert(c != NULL);

    if (c->msgused > c->hdrsize) {
        void *new_hdrbuf;
        if (c->hdrbuf)
            new_hdrbuf = realloc(c->hdrbuf, c->msgused * 2 * UDP_HEADER_SIZE);
        else
            new_hdrbuf = malloc(c->msgused * 2 * UDP_HEADER_SIZE);
        if (! new_hdrbuf)
            return -1;
        c->hdrbuf = (unsigned char *)new_hdrbuf;
        c->hdrsize = c->msgused * 2;
    }

    hdr = c->hdrbuf;
    for (i = 0; i < c->msgused; i++) {
        c->msglist[i].msg_iov[0].iov_base = (void*)hdr;
        c->msglist[i].msg_iov[0].iov_len = UDP_HEADER_SIZE;
        *hdr++ = c->request_id / 256;
        *hdr++ = c->request_id % 256;
        *hdr++ = i / 256;
        *hdr++ = i % 256;
        *hdr++ = c->msgused / 256;
        *hdr++ = c->msgused % 256;
        *hdr++ = 0;
        *hdr++ = 0;
        assert((void *) hdr == (caddr_t)c->msglist[i].msg_iov[0].iov_base + UDP_HEADER_SIZE);
    }

    return 0;
}

void out_string(conn *c, const char *str) {
    size_t len;

    assert(c != NULL);

    if (c->noreply) {
        if (settings.verbose > 1)
            fprintf(stderr, ">%d NOREPLY %s\n", c->sfd, str);
        c->noreply = false;
        conn_set_state(c, conn_new_cmd);
        return;
    }

    if (settings.verbose > 1)
        fprintf(stderr, ">%d %s\n", c->sfd, str);

    /* Nuke a partial output... */
    c->msgcurr = 0;
    c->msgused = 0;
    c->iovused = 0;
    add_msghdr(c);

    len = strlen(str);
    if ((len + 1) > c->wsize) {
        /* ought to be always enough. just fail for simplicity */
        str = "SERVER_ERROR output line too long";
        len = strlen(str);
    }

    memcpy(c->wbuf, str, len+1);
  //  memcpy(c->wbuf + len, "\x0a", 1);
    c->wbytes = len + 1;
    c->wcurr = c->wbuf;

    conn_set_state(c, conn_write);
    c->write_and_go = conn_new_cmd;
    return;
}


#if 0
void out_string(conn *c, const char *str) {
    static unsigned short len;

    assert(c != NULL);

    if (c->noreply) {
        if (settings.verbose > 1)
            fprintf(stderr, ">%d NOREPLY %s\n", c->sfd, str);
        c->noreply = false;
        conn_set_state(c, conn_new_cmd);
        return;
    }

    if (settings.verbose > 1)
        fprintf(stderr, ">%d %s\n", c->sfd, str);

    /* Nuke a partial output... */
    c->msgcurr = 0;
    c->msgused = 0;
    c->iovused = 0;
    add_msghdr(c);

    len = strlen(str);
    if ((len + 2) > c->wsize) {
        /* ought to be always enough. just fail for simplicity */
        str = "SERVER_ERROR output line too long";
        len = strlen(str);
    }
//fprintf(stderr, ">len %d\n", len);
memcpy(c->wbuf, &len, 2);
    memcpy(c->wbuf + 2, str, len);
    //memcpy(c->wbuf, str, len);
    //memcpy(c->wbuf + len, "\x0a", 1);
    c->wbytes = len + 2;
    c->wcurr = c->wbuf;

    conn_set_state(c, conn_write);
    c->write_and_go = conn_new_cmd;
    return;
}
#endif
static void append_ascii_stats(const char *key, const uint16_t klen,
                               const char *val, const uint32_t vlen,
                               conn *c) {
    char *pos = c->stats.buffer + c->stats.offset;
    uint32_t nbytes = 0;
    int remaining = c->stats.size - c->stats.offset;
    int room = remaining - 1;

    if (klen == 0 && vlen == 0) {
        nbytes = snprintf(pos, room, "END\r\n");
    } else if (vlen == 0) {
        nbytes = snprintf(pos, room, "STAT %s\r\n", key);
    } else {
        nbytes = snprintf(pos, room, "STAT %s %s\r\n", key, val);
    }

    c->stats.offset += nbytes;
}

static bool grow_stats_buf(conn *c, size_t needed) {
    size_t nsize = c->stats.size;
    size_t available = nsize - c->stats.offset;
    bool rv = true;

    /* Special case: No buffer -- need to allocate fresh */
    if (c->stats.buffer == NULL) {
        nsize = 1024;
        available = c->stats.size = c->stats.offset = 0;
    }

    while (needed > available) {
        assert(nsize > 0);
        nsize = nsize << 1;
        available = nsize - c->stats.offset;
    }

    if (nsize != c->stats.size) {
        char *ptr = (char *)realloc(c->stats.buffer, nsize);
        if (ptr) {
            c->stats.buffer = ptr;
            c->stats.size = nsize;
        } else {
            rv = false;
        }
    }

    return rv;
}

static void append_stats(const char *key, const uint16_t klen,
                  const char *val, const uint32_t vlen,
                  const void *cookie)
{
    /* value without a key is invalid */
    if (klen == 0 && vlen > 0) {
        return ;
    }

    conn *c = (conn*)cookie;

  /*  if (c->protocol == binary_prot) {
        size_t needed = vlen + klen + sizeof(protocol_binary_response_header);
        if (!grow_stats_buf(c, needed)) {
            return ;
        }
        append_bin_stats(key, klen, val, vlen, c);
    } else */{
        size_t needed = vlen + klen + 10; // 10 == "STAT = \r\n"
        if (!grow_stats_buf(c, needed)) {
            return ;
        }
        append_ascii_stats(key, klen, val, vlen, c);
    }

    assert(c->stats.offset <= c->stats.size);
}


static void reset_cmd_handler(conn *c) {
    c->substate = bin_no_state;
	
#if 0// DEL_CONN_NREAD
    if(c->item != NULL) {
        item_remove(c->item);
        c->item = NULL;
    }
#endif

    //conn_shrink(c);
    if (c->rbytes > 0) {
        conn_set_state(c, conn_parse_cmd);
    } else {
        conn_set_state(c, conn_waiting);
    }
}

typedef struct token_s {
    char *value;
    size_t length;
} token_t;

#define COMMAND_TOKEN 0
#define SUBCOMMAND_TOKEN 1
#define KEY_TOKEN 1

#define MAX_TOKENS 8

/*
 * Tokenize the command string by replacing whitespace with '\0' and update
 * the token array tokens with pointer to start of each token and length.
 * Returns total number of tokens.  The last valid token is the terminal
 * token (value points to the first unprocessed character of the string and
 * length zero).
 *
 * Usage example:
 *
 *  while(tokenize_command(command, ncommand, tokens, max_tokens) > 0) {
 *      for(int ix = 0; tokens[ix].length != 0; ix++) {
 *          ...
 *      }
 *      ncommand = tokens[ix].value - command;
 *      command  = tokens[ix].value;
 *   }
 */
static size_t tokenize_command(char *command, token_t *tokens, const size_t max_tokens) {
    char *s, *e;
    size_t ntokens = 0;

    assert(command != NULL && tokens != NULL && max_tokens > 1);

    for (s = e = command; ntokens < max_tokens - 1; ++e) {
        if (*e == ' ') {
            if (s != e) {
                tokens[ntokens].value = s;
                tokens[ntokens].length = e - s;
                ntokens++;
                *e = '\0';
            }
            s = e + 1;
        }
        else if (*e == '\0') {
            if (s != e) {
                tokens[ntokens].value = s;
                tokens[ntokens].length = e - s;
                ntokens++;
            }

            break; /* string end */
        }
    }

    /*
     * If we scanned the whole string, the terminal value pointer is null,
     * otherwise it is the first unprocessed character.
     */
    tokens[ntokens].value =  *e == '\0' ? NULL : e;
    tokens[ntokens].length = 0;
    ntokens++;

    return ntokens;
}

/* set up a connection to write a buffer then free it, used for stats */
static void write_and_free(conn *c, char *buf, int bytes) {
    if (buf) {
        c->write_and_free = buf;
        c->wcurr = buf;
        c->wbytes = bytes;
        conn_set_state(c, conn_write);
        c->write_and_go = conn_new_cmd;
    } else {
        out_string(c, "SERVER_ERROR out of memory writing stats");
    }
}

void append_stat(const char *name, ADD_STAT add_stats, conn *c,
                 const char *fmt, ...) {
    char val_str[STAT_VAL_LEN];
    int vlen;
    va_list ap;

    assert(name);
    assert(add_stats);
    assert(c);
    assert(fmt);

    va_start(ap, fmt);
    vlen = vsnprintf(val_str, sizeof(val_str) - 1, fmt, ap);
    va_end(ap);

    add_stats(name, strlen(name), val_str, vlen, c);
}

/* return server specific stats only */
static void server_stats(ADD_STAT add_stats, conn *c) {
    pid_t pid = getpid();
    rel_time_t now = current_time;

    struct thread_stats thread_stats;
    threadlocal_stats_aggregate(&thread_stats);
    struct slab_stats slab_stats;
    slab_stats_aggregate(&thread_stats, &slab_stats);

#ifndef WIN32
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
#endif /* !WIN32 */

    STATS_LOCK();

    APPEND_STAT("pid", "%lu", (long)pid);
    APPEND_STAT("uptime", "%u", now);
    APPEND_STAT("time", "%ld", now + (long)process_started);
    APPEND_STAT("version", "%s", VERSION);
    APPEND_STAT("libevent", "%s", event_get_version());
    APPEND_STAT("pointer_size", "%d", (int)(8 * sizeof(void *)));

#ifndef WIN32
    append_stat("rusage_user", add_stats, c, "%ld.%06ld",
                (long)usage.ru_utime.tv_sec,
                (long)usage.ru_utime.tv_usec);
    append_stat("rusage_system", add_stats, c, "%ld.%06ld",
                (long)usage.ru_stime.tv_sec,
                (long)usage.ru_stime.tv_usec);
#endif /* !WIN32 */

    APPEND_STAT("curr_connections", "%u", stats.curr_conns - 1);
    APPEND_STAT("total_connections", "%u", stats.total_conns);
    if (settings.maxconns_fast) {
        APPEND_STAT("rejected_connections", "%llu", (unsigned long long)stats.rejected_conns);
    }
    APPEND_STAT("connection_structures", "%u", stats.conn_structs);
    APPEND_STAT("reserved_fds", "%u", stats.reserved_fds);
	
    APPEND_STAT("bytes_read", "%llu", (unsigned long long)thread_stats.bytes_read);
    APPEND_STAT("bytes_written", "%llu", (unsigned long long)thread_stats.bytes_written);
    APPEND_STAT("accepting_conns", "%u", stats.accepting_conns);
    APPEND_STAT("listen_disabled_num", "%llu", (unsigned long long)stats.listen_disabled_num);
    APPEND_STAT("threads", "%d", settings.num_threads);
	APPEND_STAT("ad connectons", "%llu", (unsigned long long)thread_stats.flush_cmds );
	APPEND_STAT("accept_deal_time", "%llu", (unsigned long long)thread_stats.auth_cmds );
    APPEND_STAT("logic_time", "%llu", (unsigned long long)thread_stats.conn_yields);
	APPEND_STAT("avg accept_deal_time", "%llu",
		((unsigned long long)thread_stats.flush_cmds > 0 ) ? ( (unsigned long long)thread_stats.auth_cmds / (unsigned long long)thread_stats.flush_cmds ) : 0 );
    APPEND_STAT("avg logic_time", "%llu", 
			((unsigned long long)thread_stats.flush_cmds > 0 ) ? ( (unsigned long long)thread_stats.conn_yields / (unsigned long long)thread_stats.flush_cmds ) : 0 );
    APPEND_STAT("avg time", "%llu",
		((unsigned long long)thread_stats.flush_cmds > 0 ) ? (((unsigned long long)thread_stats.conn_yields +(unsigned long long)thread_stats.auth_cmds) / (unsigned long long)thread_stats.flush_cmds  ) : 0 );
	
 //   APPEND_STAT("hash_power_level", "%u", stats.hash_power_level);
 //   APPEND_STAT("hash_bytes", "%llu", (unsigned long long)stats.hash_bytes);
 //   APPEND_STAT("hash_is_expanding", "%u", stats.hash_is_expanding);
 //   APPEND_STAT("expired_unfetched", "%llu", stats.expired_unfetched);
 //   APPEND_STAT("evicted_unfetched", "%llu", stats.evicted_unfetched);
    STATS_UNLOCK();
}

static void process_stat_settings(ADD_STAT add_stats, void *c) {
    assert(add_stats);
    APPEND_STAT("maxconns", "%d", settings.maxconns);
    APPEND_STAT("tcpport", "%d", settings.port);
    APPEND_STAT("udpport", "%d", settings.udpport);
    APPEND_STAT("inter", "%s", settings.inter ? settings.inter : "NULL");
    APPEND_STAT("verbosity", "%d", settings.verbose);
    APPEND_STAT("oldest", "%lu", (unsigned long)settings.oldest_live);
    APPEND_STAT("domain_socket", "%s",
                settings.socketpath ? settings.socketpath : "NULL");
    APPEND_STAT("umask", "%o", settings.access);
    APPEND_STAT("num_threads", "%d", settings.num_threads);
    APPEND_STAT("num_threads_per_udp", "%d", settings.num_threads_per_udp);
    APPEND_STAT("detail_enabled", "%s",
                settings.detail_enabled ? "yes" : "no");
    APPEND_STAT("reqs_per_event", "%d", settings.reqs_per_event);
    APPEND_STAT("cas_enabled", "%s", settings.use_cas ? "yes" : "no");
    APPEND_STAT("tcp_backlog", "%d", settings.backlog);
    APPEND_STAT("binding_protocol", "%s",
                prot_text(settings.binding_protocol));
    APPEND_STAT("auth_enabled_sasl", "%s", settings.sasl ? "yes" : "no");
    APPEND_STAT("maxconns_fast", "%s", settings.maxconns_fast ? "yes" : "no");
    APPEND_STAT("hashpower_init", "%d", settings.hashpower_init);
}

static void process_stat(conn *c, token_t *tokens, const size_t ntokens) {
    const char *subcommand = tokens[SUBCOMMAND_TOKEN].value;
    assert(c != NULL);

    if (ntokens < 2) {
        out_string(c, "CLIENT_ERROR bad command line");
        return;
    }

    if (ntokens == 2) {
        server_stats(&append_stats, c);
    } 
	else if (strcmp(subcommand, "settings") == 0) {
        process_stat_settings(&append_stats, c);
    }
	else {
		out_string(c, "ERROR");
		return ;
    }

    /* append terminator and start the transfer */
    append_stats(NULL, 0, NULL, 0, c);

    if (c->stats.buffer == NULL) {
        out_string(c, "SERVER_ERROR out of memory writing stats");
    } else {
        write_and_free(c, c->stats.buffer, c->stats.offset);
        c->stats.buffer = NULL;
    }
}



void process_command(conn *c, char *command) {

    token_t tokens[MAX_TOKENS];
    size_t ntokens;
   // int comm;

    assert(c != NULL);


  if (settings.verbose > 1)
        fprintf(stderr, "<%d %s\n", c->sfd, command);

    /*
     * for commands set/add/replace, we build an item and read the data
     * directly into it, then continue in nread_complete().
     */

    c->msgcurr = 0;
    c->msgused = 0;
    c->iovused = 0;
    if (add_msghdr(c) != 0) {
        out_string(c, "SERVER_ERROR out of memory preparing response");
        return;
    }

	struct ais_connection *ac = (ais_connection*)(c->bulk);
	ac->bufr = command;
	ac->bufr_len = c->rbytes;
	ac->c = c;
	if( c->rbytes >2 && ac->bufr && 
		( ( 'a' == ac->bufr[0] || ac->bufr[0] == 'c'||ac->bufr[0]=='r' ) && ac->bufr[1] == '&' ) ){
		reset_per_request_attributes(ac); 
		if(ac->bufr &&ac->bufr[0] == 'a'){
			ais_get(ac);
		}
		//else if(ac->bufr &&ac->bufr[0] == 'b'){
	//		ais_getbody(ac);
		//}
		else if(ac->bufr&&ac->bufr[0] == 'c'){
			ais_wap(ac);
		}
		else{
			ais_report(ac);
			}
            pthread_mutex_lock(&c->thread->stats.mutex);
			c->thread->stats.flush_cmds    ++ ;
			
#ifdef DEBUG_TIME	

			c->thread->stats.conn_yields   += (ac->end - ac->start) ;
			c->thread->stats.auth_cmds += (ac->start- ac->org_start);
#endif 	
            pthread_mutex_unlock(&c->thread->stats.mutex);
			
		return;
    }

    if ( (NULL!=ac->bufr)
        && (c->rbytes>strlen("cgi=imp&"))
        && (c->rbytes>strlen("cgi=clk&"))
        && ((strncasecmp((char*)"cgi=imp&", ac->bufr, strlen("cgi=imp&")) == 0)
            || (strncasecmp((char*)"cgi=clk&", ac->bufr, strlen("cgi=clk&")) == 0) ))
    {   
        ais_imp_clk(ac);
        return;
    }

    if ( (NULL!=ac->bufr)
        && (c->rbytes>strlen("cgi=cbimp&"))
        && (c->rbytes>strlen("cgi=cbclk&"))
        && ((strncasecmp((char*)"cgi=cbimp&", ac->bufr, strlen("cgi=cbimp&")) == 0)
            || (strncasecmp((char*)"cgi=cbclk&", ac->bufr, strlen("cgi=cbclk&")) == 0) ))
    {   
        struct tm date = {0};
        ais_callback(ac, 0,&date);
		return;
		}
    if(( c->rbytes >2 && ac->bufr &&                /* h is oflshow; i is oflclick */
		(('h' == ac->bufr[0] || 'i'==ac->bufr[0]) && ac->bufr[1] == '&' ) )){
		ais_imp_clk_new(ac);
		return;
		}
	if(( c->rbytes >2 && ac->bufr && 
		( 'd' == ac->bufr[0] && ac->bufr[1] == '&' ) ))
	command = "stats";
	ntokens = tokenize_command(command, tokens, MAX_TOKENS);
	if (ntokens >= 2 && (strcmp(tokens[COMMAND_TOKEN].value, "stats") == 0)) {
   process_stat(c, tokens, ntokens);
		// out_string(c, "ERRORtest");
	}
	else{
	    out_string(c, "ERROR");
	}
    return;
}

/*
 * if we have a complete line in the buffer, process it.
 */
static int try_read_command(conn *c) {
    assert(c != NULL);
    assert(c->rcurr <= (c->rbuf + c->rsize));
    //assert(c->rbytes > 0);


	

 {
        char *el, *cont;

        if (c->rbytes == 0)
            return 0;

        el = (char*)memchr(c->rcurr, '\n', c->rbytes);
        if (!el) {
            if (c->rbytes > 1024) {
                /*
                 * We didn't have a '\n' in the first k. This _has_ to be a
                 * large multiget, if not we should just nuke the connection.
                 */
                char *ptr = c->rcurr;
                while (*ptr == ' ') { /* ignore leading whitespaces */
                    ++ptr;
                }

                if (ptr - c->rcurr > 100 ||
                    (strncmp(ptr, "get ", 4) && strncmp(ptr, "gets ", 5))) {

                    conn_set_state(c, conn_closing);
                    return 1;
                }
            }

            return 0;
        }
        cont = el + 1;
        if ((el - c->rcurr) > 1 && *(el - 1) == '\r') {
            el--;
        }
        *el = '\0';

        assert(cont <= (c->rcurr + c->rbytes));

        process_command(c, c->rcurr);

        c->rbytes -= (cont - c->rcurr);
        c->rcurr = cont;

        assert(c->rcurr <= (c->rbuf + c->rsize));
    }

    return 1;
}

/*
 * read a UDP request.
 */
static enum try_read_result try_read_udp(conn *c) {
    int res;

    assert(c != NULL);

    c->request_addr_size = sizeof(c->request_addr);
    res = recvfrom(c->sfd, c->rbuf, c->rsize,
                   0, &c->request_addr, &c->request_addr_size);
    if (res > 8) {
        unsigned char *buf = (unsigned char *)c->rbuf;
        pthread_mutex_lock(&c->thread->stats.mutex);
        c->thread->stats.bytes_read += res;
        pthread_mutex_unlock(&c->thread->stats.mutex);

        /* Beginning of UDP packet is the request ID; save it. */
        c->request_id = buf[0] * 256 + buf[1];

        /* If this is a multi-packet request, drop it. */
        if (buf[4] != 0 || buf[5] != 1) {
            out_string(c, "SERVER_ERROR multi-packet request not supported");
            return READ_NO_DATA_RECEIVED;
        }

        /* Don't care about any of the rest of the header. */
        res -= 8;
        memmove(c->rbuf, c->rbuf + 8, res);

        c->rbytes = res;
        c->rcurr = c->rbuf;
        return READ_DATA_RECEIVED;
    }
    return READ_NO_DATA_RECEIVED;
}

/*
 * read from network as much as we can, handle buffer overflow and connection
 * close.
 * before reading, move the remaining incomplete fragment of a command
 * (if any) to the beginning of the buffer.
 *
 * To protect us from someone flooding a connection with bogus data causing
 * the connection to eat up all available memory, break out and start looking
 * at the data I've got after a number of reallocs...
 *
 * @return enum try_read_result
 */
static enum try_read_result try_read_network(conn *c) {
    enum try_read_result gotdata = READ_NO_DATA_RECEIVED;
    int res;
    int num_allocs = 0;
    assert(c != NULL);

    if (c->rcurr != c->rbuf) {
        if (c->rbytes != 0) /* otherwise there's nothing to copy */
            memmove(c->rbuf, c->rcurr, c->rbytes);
        c->rcurr = c->rbuf;
    }

    while (1) {
        if (c->rbytes >= c->rsize) {
            if (num_allocs == 4) {
                return gotdata;
            }
            ++num_allocs;
            char *new_rbuf = (char *)realloc(c->rbuf, c->rsize * 2);
            if (!new_rbuf) {
                if (settings.verbose > 0)
                    fprintf(stderr, "Couldn't realloc input buffer\n");
                c->rbytes = 0; /* ignore what we read */
                out_string(c, "SERVER_ERROR out of memory reading request");
                c->write_and_go = conn_closing;
                return READ_MEMORY_ERROR;
            }
            c->rcurr = c->rbuf = new_rbuf;
            c->rsize *= 2;
        }

        int avail = c->rsize - c->rbytes;
        res = read(c->sfd, c->rbuf + c->rbytes, avail);
        if (res > 0) {
            pthread_mutex_lock(&c->thread->stats.mutex);
            c->thread->stats.bytes_read += res;
            pthread_mutex_unlock(&c->thread->stats.mutex);
            gotdata = READ_DATA_RECEIVED;
            c->rbytes += res;
            if (res == avail) {
                continue;
            } else {
                break;
            }
        }
        if (res == 0) {
            return READ_ERROR;
        }
        if (res == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            return READ_ERROR;
        }
    }
    return gotdata;
}

static bool update_event(conn *c, const int new_flags) {
    assert(c != NULL);

    struct event_base *base = c->event.ev_base;
    if (c->ev_flags == new_flags)
        return true;
    if (event_del(&c->event) == -1) return false;
    event_set(&c->event, c->sfd, new_flags, event_handler, (void *)c);
    event_base_set(base, &c->event);
    c->ev_flags = new_flags;
    if (event_add(&c->event, 0) == -1) return false;
    return true;
}

/*
 * Sets whether we are listening for new connections or not.
 */
void do_accept_new_conns(const bool do_accept) {
    conn *next;

    for (next = listen_conn; next; next = next->next) {
        if (do_accept) {
            update_event(next, EV_READ | EV_PERSIST);
            if (listen(next->sfd, settings.backlog) != 0) {
                perror("listen");
            }
        }
        else {
            update_event(next, 0);
            if (listen(next->sfd, 0) != 0) {
                perror("listen");
            }
        }
    }

    if (do_accept) {
        STATS_LOCK();
        stats.accepting_conns = true;
        STATS_UNLOCK();
    } else {
        STATS_LOCK();
        stats.accepting_conns = false;
        stats.listen_disabled_num++;
        STATS_UNLOCK();
        allow_new_conns = false;
        maxconns_handler(-42, 0, 0);
    }
}

/*
 * Transmit the next chunk of data from our list of msgbuf structures.
 *
 * Returns:
 *   TRANSMIT_COMPLETE   All done writing.
 *   TRANSMIT_INCOMPLETE More data remaining to write.
 *   TRANSMIT_SOFT_ERROR Can't write any more right now.
 *   TRANSMIT_HARD_ERROR Can't write (c->state is set to conn_closing)
 */
static enum transmit_result transmit(conn *c) {
    assert(c != NULL);

    if (c->msgcurr < c->msgused &&
            c->msglist[c->msgcurr].msg_iovlen == 0) {
        /* Finished writing the current msg; advance to the next. */
        c->msgcurr++;
    }
    if (c->msgcurr < c->msgused) {
        ssize_t res;
        struct msghdr *m = &c->msglist[c->msgcurr];

        res = sendmsg(c->sfd, m, 0);
        if (res > 0) {
            pthread_mutex_lock(&c->thread->stats.mutex);
            c->thread->stats.bytes_written += res;
            pthread_mutex_unlock(&c->thread->stats.mutex);

            /* We've written some of the data. Remove the completed
               iovec entries from the list of pending writes. */
            while (m->msg_iovlen > 0 && res >= m->msg_iov->iov_len) {
                res -= m->msg_iov->iov_len;
                m->msg_iovlen--;
                m->msg_iov++;
            }

            /* Might have written just part of the last iovec entry;
               adjust it so the next write will do the rest. */
            if (res > 0) {
                m->msg_iov->iov_base = (caddr_t)m->msg_iov->iov_base + res;
                m->msg_iov->iov_len -= res;
            }
            return TRANSMIT_INCOMPLETE;
        }
        if (res == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            if (!update_event(c, EV_WRITE | EV_PERSIST)) {
                if (settings.verbose > 0)
                    fprintf(stderr, "Couldn't update event\n");
                conn_set_state(c, conn_closing);
                return TRANSMIT_HARD_ERROR;
            }
            return TRANSMIT_SOFT_ERROR;
        }
        /* if res == 0 or res == -1 and error is not EAGAIN or EWOULDBLOCK,
           we have a real error, on which we close the connection */
        if (settings.verbose > 0)
            perror("Failed to write, and not due to blocking");

        if (IS_UDP(c->transport))
            conn_set_state(c, conn_read);
        else
            conn_set_state(c, conn_closing);
        return TRANSMIT_HARD_ERROR;
    } else {
        return TRANSMIT_COMPLETE;
    }
}

static void drive_machine(conn *c) {
    bool stop = false;
    int sfd, flags = 1;
    socklen_t addrlen;
    struct sockaddr_storage addr;
    int nreqs = settings.reqs_per_event;
    int res;
    const char *str;
	struct ais_connection *ac;
    assert(c != NULL);

    while (!stop) {

        switch(c->state) {
        case conn_listening:
            addrlen = sizeof(addr);

            if ((sfd = accept(c->sfd, (struct sockaddr *)&addr, &addrlen)) == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    /* these are transient, so don't log anything */
                    stop = true;
                } else if (errno == EMFILE) {
                    if (settings.verbose > 0)
                        fprintf(stderr, "Too many open connections\n");
                    accept_new_conns(false);
                    stop = true;
                } else {
                    perror("accept()");
                    stop = true;
                }
                break;
            }
			
            if ((flags = fcntl(sfd, F_GETFL, 0)) < 0 ||
                fcntl(sfd, F_SETFL, flags | O_NONBLOCK) < 0) {
                perror("setting O_NONBLOCK");
                close(sfd);
                break;
            }

            if (settings.maxconns_fast &&
                stats.curr_conns + stats.reserved_fds >= settings.maxconns - 1) {
                str = "ERROR Too many open connections\r\n";
                res = write(sfd, str, strlen(str));
                close(sfd);
                STATS_LOCK();
                stats.rejected_conns++;
                STATS_UNLOCK();
            } else {
                dispatch_conn_new(sfd, conn_new_cmd, EV_READ | EV_PERSIST,
                                     DATA_BUFFER_SIZE, tcp_transport);
            }

            stop = true;
            break;

        case conn_waiting:
            if (!update_event(c, EV_READ | EV_PERSIST)) {
                if (settings.verbose > 0)
                    fprintf(stderr, "Couldn't update event\n");
                conn_set_state(c, conn_closing);
                break;
            }

            conn_set_state(c, conn_read);
            stop = true;
            break;

        case conn_read:
            res = IS_UDP(c->transport) ? try_read_udp(c) : try_read_network(c);

            switch (res) {
            case READ_NO_DATA_RECEIVED:
                conn_set_state(c, conn_waiting);
                break;
            case READ_DATA_RECEIVED:
                conn_set_state(c, conn_parse_cmd);
                break;
            case READ_ERROR:
                conn_set_state(c, conn_closing);
                break;
            case READ_MEMORY_ERROR: /* Failed to allocate more memory */
                /* State already set by try_read_network */
                break;
            }
            break;

        case conn_parse_cmd :
            if (try_read_command(c) == 0) {
                /* wee need more data! */
                conn_set_state(c, conn_waiting);
            }

            break;

        case conn_new_cmd:
            /* Only process nreqs at a time to avoid starving other
               connections */
			ac = (ais_connection*)(c->bulk);
#ifdef DEBUG_TIME

			ac->start = getmocrtime(); //add for time debug
#endif				
			
		//	GETTIMEOFDAY(&ac->start );
            --nreqs;
            if (nreqs >= 0) {
                reset_cmd_handler(c);
            } else {
#ifdef DEBUG_TIME	
				pthread_mutex_lock(&c->thread->stats.mutex);
                c->thread->stats.conn_yields++;
                pthread_mutex_unlock(&c->thread->stats.mutex);
#endif				
                if (c->rbytes > 0) {
                    /* We have already read in data into the input buffer,
                       so libevent will most likely not signal read events
                       on the socket (unless more data is available. As a
                       hack we should just put in a request to write data,
                       because that should be possible ;-)
                    */
                    if (!update_event(c, EV_WRITE | EV_PERSIST)) {
                        if (settings.verbose > 0)
                            fprintf(stderr, "Couldn't update event\n");
                        conn_set_state(c, conn_closing);
                    }
                }
                stop = true;
            }
            break;
#if 0// DEL_CONN_NREAD
        case conn_nread:
			 fprintf(stderr, "<<<<<<<<<<<<<<<<<< why !!!!!!!!conn_nread\n");
		//	 exit(EX_OSERR);
            if (c->rlbytes == 0) {
                complete_nread(c);
                break;
            }
            /* first check if we have leftovers in the conn_read buffer */
            if (c->rbytes > 0) {
                int tocopy = c->rbytes > c->rlbytes ? c->rlbytes : c->rbytes;
                if (c->ritem != c->rcurr) {
                    memmove(c->ritem, c->rcurr, tocopy);
                }
                c->ritem += tocopy;
                c->rlbytes -= tocopy;
                c->rcurr += tocopy;
                c->rbytes -= tocopy;
                if (c->rlbytes == 0) {
                    break;
                }
            }

            /*  now try reading from the socket */
            res = read(c->sfd, c->ritem, c->rlbytes);
            if (res > 0) {
                pthread_mutex_lock(&c->thread->stats.mutex);
                c->thread->stats.bytes_read += res;
                pthread_mutex_unlock(&c->thread->stats.mutex);
                if (c->rcurr == c->ritem) {
                    c->rcurr += res;
                }
                c->ritem += res;
                c->rlbytes -= res;
                break;
            }
            if (res == 0) { /* end of stream */
                conn_set_state(c, conn_closing);
                break;
            }
            if (res == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                if (!update_event(c, EV_READ | EV_PERSIST)) {
                    if (settings.verbose > 0)
                        fprintf(stderr, "Couldn't update event\n");
                    conn_set_state(c, conn_closing);
                    break;
                }
                stop = true;
                break;
            }
            /* otherwise we have a real error, on which we close the connection */
            if (settings.verbose > 0) {
                fprintf(stderr, "Failed to read, and not due to blocking:\n"
                        "errno: %d %s \n"
                        "rcurr=%lx ritem=%lx rbuf=%lx rlbytes=%d rsize=%d\n",
                        errno, strerror(errno),
                        (long)c->rcurr, (long)c->ritem, (long)c->rbuf,
                        (int)c->rlbytes, (int)c->rsize);
            }
            conn_set_state(c, conn_closing);
            break;
#endif
        case conn_swallow:
            /* we are reading sbytes and throwing them away */
            if (c->sbytes == 0) {
                conn_set_state(c, conn_new_cmd);
                break;
            }

            /* first check if we have leftovers in the conn_read buffer */
            if (c->rbytes > 0) {
                int tocopy = c->rbytes > c->sbytes ? c->sbytes : c->rbytes;
                c->sbytes -= tocopy;
                c->rcurr += tocopy;
                c->rbytes -= tocopy;
                break;
            }

            /*  now try reading from the socket */
            res = read(c->sfd, c->rbuf, c->rsize > c->sbytes ? c->sbytes : c->rsize);
            if (res > 0) {
                pthread_mutex_lock(&c->thread->stats.mutex);
                c->thread->stats.bytes_read += res;
                pthread_mutex_unlock(&c->thread->stats.mutex);
                c->sbytes -= res;
                break;
            }
            if (res == 0) { /* end of stream */
                conn_set_state(c, conn_closing);
                break;
            }
            if (res == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                if (!update_event(c, EV_READ | EV_PERSIST)) {
                    if (settings.verbose > 0)
                        fprintf(stderr, "Couldn't update event\n");
                    conn_set_state(c, conn_closing);
                    break;
                }
                stop = true;
                break;
            }
            /* otherwise we have a real error, on which we close the connection */
            if (settings.verbose > 0)
                fprintf(stderr, "Failed to read, and not due to blocking\n");
            conn_set_state(c, conn_closing);
            break;

        case conn_write:
            /*
             * We want to write out a simple response. If we haven't already,
             * assemble it into a msgbuf list (this will be a single-entry
             * list for TCP or a two-entry list for UDP).
             */
            if (c->iovused == 0 || (IS_UDP(c->transport) && c->iovused == 1)) {
                if (add_iov(c, c->wcurr, c->wbytes) != 0) {
                    if (settings.verbose > 0)
                        fprintf(stderr, "Couldn't build response\n");
                    conn_set_state(c, conn_closing);
                    break;
                }
            }

            /* fall through... */

        case conn_mwrite:
          if (IS_UDP(c->transport) && c->msgcurr == 0 && build_udp_headers(c) != 0) {
            if (settings.verbose > 0)
              fprintf(stderr, "Failed to build UDP headers\n");
            conn_set_state(c, conn_closing);
            break;
          }
            switch (transmit(c)) {
            case TRANSMIT_COMPLETE:
                if (c->state == conn_mwrite) {

                    /* XXX:  I don't know why this wasn't the general case */
                    if(c->protocol == binary_prot) {
                        conn_set_state(c, c->write_and_go);
                    } else {
                        conn_set_state(c, conn_new_cmd);
                    }
                } else if (c->state == conn_write) {
                    if (c->write_and_free) {
                        free(c->write_and_free);
                        c->write_and_free = 0;
                    }
                    conn_set_state(c, c->write_and_go);
                } else {
                    if (settings.verbose > 0)
                        fprintf(stderr, "Unexpected state %d\n", c->state);
                    conn_set_state(c, conn_closing);
                }
                break;

            case TRANSMIT_INCOMPLETE:
            case TRANSMIT_HARD_ERROR:
                break;                   /* Continue in state machine. */

            case TRANSMIT_SOFT_ERROR:
                stop = true;
                break;
            }
            break;

        case conn_closing:
            if (IS_UDP(c->transport))
                conn_cleanup(c);
            else
                conn_close(c);
            stop = true;
            break;

        case conn_max_state:
            assert(false);
            break;
        }
    }

    return;
}

void event_handler(const int fd, const short which, void *arg) {
    conn *c;

    c = (conn *)arg;
    assert(c != NULL);

    c->which = which;

    /* sanity */
    if (fd != c->sfd) {
        if (settings.verbose > 0)
            fprintf(stderr, "Catastrophic: event fd doesn't match conn fd!\n");
        conn_close(c);
        return;
    }

    drive_machine(c);

    /* wait for next event */
    return;
}

static int new_socket(struct addrinfo *ai) {
    int sfd;
    int flags;

    if ((sfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) == -1) {
        return -1;
    }

    if ((flags = fcntl(sfd, F_GETFL, 0)) < 0 ||
        fcntl(sfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("setting O_NONBLOCK");
        close(sfd);
        return -1;
    }
    return sfd;
}


/*
 * Sets a socket's send buffer size to the maximum allowed by the system.
 */
static void maximize_sndbuf(const int sfd) {
    socklen_t intsize = sizeof(int);
    int last_good = 0;
    int min, max, avg;
    int old_size;

    /* Start with the default size. */
    if (getsockopt(sfd, SOL_SOCKET, SO_SNDBUF, &old_size, &intsize) != 0) {
        if (settings.verbose > 0)
            perror("getsockopt(SO_SNDBUF)");
        return;
    }

    /* Binary-search for the real maximum. */
    min = old_size;
    max = MAX_SENDBUF_SIZE;

    while (min <= max) {
        avg = ((unsigned int)(min + max)) / 2;
        if (setsockopt(sfd, SOL_SOCKET, SO_SNDBUF, (void *)&avg, intsize) == 0) {
            last_good = avg;
            min = avg + 1;
        } else {
            max = avg - 1;
        }
    }

    if (settings.verbose > 1)
        fprintf(stderr, "<%d send buffer was %d, now %d\n", sfd, old_size, last_good);
}

/**
 * Create a socket and bind it to a specific port number
 * @param interface the interface to bind to
 * @param port the port number to bind to
 * @param transport the transport protocol (TCP / UDP)
 * @param portnumber_file A filepointer to write the port numbers to
 *        when they are successfully added to the list of ports we
 *        listen on.
 */
static int server_socket(const char *interface,
                         int port,
                         enum network_transport transport,
                         FILE *portnumber_file, int tid) {
    int sfd;
    struct linger ling = {0, 0};
    struct addrinfo *ai;
    struct addrinfo *next;
    struct addrinfo hints;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    
    char port_buf[NI_MAXSERV];
    int error;
    int success = 0;
    int flags =1;

    hints.ai_socktype = IS_UDP(transport) ? SOCK_DGRAM : SOCK_STREAM;

    if (port == -1) {
        port = 0;
    }
    snprintf(port_buf, sizeof(port_buf), "%d", port);
    error= getaddrinfo(interface, port_buf, &hints, &ai);
    if (error != 0) {
        if (error != EAI_SYSTEM)
          fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(error));
        else
          perror("getaddrinfo()");
        return 1;
    }

    for (next= ai; next; next= next->ai_next) {
        conn *listen_conn_add;
        if ((sfd = new_socket(next)) == -1) {
            /* getaddrinfo can return "junk" addresses,
             * we make sure at least one works before erroring.
             */
            if (errno == EMFILE) {
                /* ...unless we're out of fds */
                perror("server_socket");
                exit(EX_OSERR);
            }
            continue;
        }

#ifdef IPV6_V6ONLY
        if (next->ai_family == AF_INET6) {
            error = setsockopt(sfd, IPPROTO_IPV6, IPV6_V6ONLY, (char *) &flags, sizeof(flags));
            if (error != 0) {
                perror("setsockopt");
                close(sfd);
                continue;
            }
        }
#endif

        setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void *)&flags, sizeof(flags));
        if (IS_UDP(transport)) {
            maximize_sndbuf(sfd);
        } else {
            error = setsockopt(sfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&flags, sizeof(flags));
            if (error != 0)
                perror("setsockopt");

            error = setsockopt(sfd, SOL_SOCKET, SO_LINGER, (void *)&ling, sizeof(ling));
            if (error != 0)
                perror("setsockopt");

            error = setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flags, sizeof(flags));
            if (error != 0)
                perror("setsockopt");
        }

        if (bind(sfd, next->ai_addr, next->ai_addrlen) == -1) {
            if (errno != EADDRINUSE) {
                perror("bind()");
                close(sfd);
                freeaddrinfo(ai);
                return 1;
            }
            close(sfd);
            continue;
        } else {
            success++;
            if (!IS_UDP(transport) && listen(sfd, settings.backlog) == -1) {
                perror("listen()");
                close(sfd);
                freeaddrinfo(ai);
                return 1;
            }
            if (portnumber_file != NULL &&
                (next->ai_addr->sa_family == AF_INET ||
                 next->ai_addr->sa_family == AF_INET6)) {
                union {
                    struct sockaddr_in in;
                    struct sockaddr_in6 in6;
                } my_sockaddr;
                socklen_t len = sizeof(my_sockaddr);
                if (getsockname(sfd, (struct sockaddr*)&my_sockaddr, &len)==0) {
                    if (next->ai_addr->sa_family == AF_INET) {
                        fprintf(portnumber_file, "%s INET: %u\n",
                                IS_UDP(transport) ? "UDP" : "TCP",
                                ntohs(my_sockaddr.in.sin_port));
                    } else {
                        fprintf(portnumber_file, "%s INET6: %u\n",
                                IS_UDP(transport) ? "UDP" : "TCP",
                                ntohs(my_sockaddr.in6.sin6_port));
                    }
                }
            }
        }

        if (IS_UDP(transport)) {
            int c;

            for (c = 0; c < settings.num_threads_per_udp; c++) {
                /* this is guaranteed to hit all threads because we round-robin */
                dispatch_conn_new(sfd, conn_read, EV_READ | EV_PERSIST,
                                  UDP_READ_BUFFER_SIZE, transport);
            }
        } else {
            if (!(listen_conn_add = conn_new(sfd, conn_listening,
                                             EV_READ | EV_PERSIST, 1,
                                             transport, main_base,0, tid))) { // modify for debug time
                fprintf(stderr, "failed to create listening connection\n");
                exit(EXIT_FAILURE);
            }
            listen_conn_add->next = listen_conn;
            listen_conn = listen_conn_add;
        }
    }

    freeaddrinfo(ai);

    /* Return zero iff we detected no errors in starting up connections */
    return success == 0;
}

static int server_sockets(int port, enum network_transport transport,
                          FILE *portnumber_file, int tid) {
    if (settings.inter == NULL) {
        return server_socket(settings.inter, port, transport, portnumber_file, tid);
    } else {
        // tokenize them and bind to each one of them..
        char *b;
        int ret = 0;
        char *list = strdup(settings.inter);

        if (list == NULL) {
            fprintf(stderr, "Failed to allocate memory for parsing server interface string\n");
            return 1;
        }
        for (char *p = strtok_r(list, ";,", &b);
             p != NULL;
             p = strtok_r(NULL, ";,", &b)) {
            int the_port = port;
            char *s = strchr(p, ':');
            if (s != NULL) {
                *s = '\0';
                ++s;
                if (!safe_strtol(s, &the_port)) {
                    fprintf(stderr, "Invalid port number: \"%s\"", s);
                    return 1;
                }
            }
            if (strcmp(p, "*") == 0) {
                p = NULL;
            }
            ret |= server_socket(p, the_port, transport, portnumber_file, tid);
        }
        free(list);
        return ret;
    }
}

static int new_socket_unix(void) {
    int sfd;
    int flags;

    if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        return -1;
    }

    if ((flags = fcntl(sfd, F_GETFL, 0)) < 0 ||
        fcntl(sfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("setting O_NONBLOCK");
        close(sfd);
        return -1;
    }
    return sfd;
}

static int server_socket_unix(const char *path, int access_mask, int tid) {
    int sfd;
    struct linger ling = {0, 0};
    struct sockaddr_un addr;
    struct stat tstat;
    int flags =1;
    int old_umask;

    if (!path) {
        return 1;
    }

    if ((sfd = new_socket_unix()) == -1) {
        return 1;
    }

    /*
     * Clean up a previous socket file if we left it around
     */
    if (lstat(path, &tstat) == 0) {
        if (S_ISSOCK(tstat.st_mode))
            unlink(path);
    }

    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void *)&flags, sizeof(flags));
    setsockopt(sfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&flags, sizeof(flags));
    setsockopt(sfd, SOL_SOCKET, SO_LINGER, (void *)&ling, sizeof(ling));

    /*
     * the memset call clears nonstandard fields in some impementations
     * that otherwise mess things up.
     */
    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    assert(strcmp(addr.sun_path, path) == 0);
    old_umask = umask( ~(access_mask&0777));
    if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind()");
        close(sfd);
        umask(old_umask);
        return 1;
    }
    umask(old_umask);
    if (listen(sfd, settings.backlog) == -1) {
        perror("listen()");
        close(sfd);
        return 1;
    }
    if (!(listen_conn = conn_new(sfd, conn_listening,
                                 EV_READ | EV_PERSIST, 1,
                                 local_transport, main_base, 0, tid))) { //for debut time
        fprintf(stderr, "failed to create listening connection\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

/*
 * We keep the current time of day in a global variable that's updated by a
 * timer event. This saves us a bunch of time() system calls (we really only
 * need to get the time once a second, whereas there can be tens of thousands
 * of requests a second) and allows us to use server-start-relative timestamps
 * rather than absolute UNIX timestamps, a space savings on systems where
 * sizeof(time_t) > sizeof(unsigned int).
 */
volatile rel_time_t current_time;
static struct event clockevent;

/* libevent uses a monotonic clock when available for event scheduling. Aside
 * from jitter, simply ticking our internal timer here is accurate enough.
 * Note that users who are setting explicit dates for expiration times *must*
 * ensure their clocks are correct before starting ais. */
static void clock_handler(const int fd, const short which, void *arg) {
    struct timeval t;
	t.tv_sec = 1;
	t.tv_usec = 0;
    
    static bool initialized = false;
#if defined(HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
    static bool monotonic = false;
    static time_t monotonic_start;
#endif

    if (initialized) {
        /* only delete the event if it's actually there. */
        evtimer_del(&clockevent);
    } else {
        initialized = true;
        /* process_started is initialized to time() - 2. We initialize to 1 so
         * flush_all won't underflow during tests. */
#if defined(HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
            monotonic = true;
            monotonic_start = ts.tv_sec - 2;
        }
#endif
    }

    evtimer_set(&clockevent, clock_handler, 0);
    event_base_set(main_base, &clockevent);
    evtimer_add(&clockevent, &t);

#if defined(HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
    if (monotonic) {
        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
            return;
        current_time = (rel_time_t) (ts.tv_sec - monotonic_start);
        return;
    }
#endif
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        current_time = (rel_time_t) (tv.tv_sec - process_started);
    }
}

static void save_pid(const char *pid_file) {
    FILE *fp;
    if (access(pid_file, F_OK) == 0) {
        if ((fp = fopen(pid_file, "r")) != NULL) {
            char buffer[1024];
            if (fgets(buffer, sizeof(buffer), fp) != NULL) {
                unsigned int pid;
                if (safe_strtoul(buffer, &pid) && kill((pid_t)pid, 0) == 0) {
                    fprintf(stderr, "WARNING: The pid file contained the following (running) pid: %u\n", pid);
                }
            }
            fclose(fp);
        }
    }

    if ((fp = fopen(pid_file, "w")) == NULL) {
        vperror("Could not open the pid file %s for writing", pid_file);
        return;
    }

    fprintf(fp,"%ld\n", (long)getpid());
    if (fclose(fp) == -1) {
        vperror("Could not close the pid file %s", pid_file);
    }
}

static void remove_pidfile(const char *pid_file) {
  if (pid_file == NULL)
      return;

  if (unlink(pid_file) != 0) {
      vperror("Could not remove the pid file %s", pid_file);
  }

}

struct redisContext* get_redis_conn()
{
    char *unixpath = NULL;
    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    struct redisContext* redisctx = NULL;

    if(g_ctx.config[REDIS_IP] != NULL)
    {
        if((unixpath = strstr(g_ctx.config[REDIS_IP],"unix:")) != NULL)
        {	
            redisctx=redisConnectUnixWithTimeout(unixpath+sizeof("unix:")-1, timeout) ;
            if (redisctx->err) {  
                fprintf(stderr, "redisConnectUnixWithTimeout()\n");
                redisFree(redisctx);
                return NULL;
            }
        }
        else
        {
            redisctx= redisConnectWithTimeout((char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT]), timeout);
            if (redisctx->err) {  
                fprintf(stderr, "redisConnectWithTimeout(), ip:%s, port:%d\n", (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT]));
                redisFree(redisctx);
                return NULL;
            }
        }
    }
    else
    {
        redisctx= redisConnectWithTimeout((char*)"127.0.0.1", 6379, timeout);
        if (redisctx->err) {  
            fprintf(stderr, "redisConnectWithTimeout() %s, ip:%s, port:%d\n",redisctx->errstr, (char*)"127.0.0.1", 6379);
            redisFree(redisctx);
            return NULL;
        }
    }

    if (cliAuth(redisctx) != REDIS_OK)
    {
        fprintf(stderr, "log_work_thread cliAuth failed \n" );
        return NULL;
    }

    struct timeval tval;
    tval.tv_sec=2;
    tval.tv_usec=0;
    redisSetTimeout(redisctx, tval);

    redisctx->flags |= REDIS_CONNECTED;
    return redisctx;
}

struct redisContext* get_sredis_conn(int port)
{
    char *unixpath = NULL;
    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    struct redisContext* redisctx = NULL;

    if(g_ctx.config[REDIS_SLAVE_IP] != NULL)
    {
        if((unixpath = strstr(g_ctx.config[REDIS_SLAVE_IP],"unix:")) != NULL)
        {	
            redisctx = redisConnectUnixWithTimeout(unixpath+sizeof("unix:")-1, timeout) ;
            if (redisctx->err) {  
                DEBUG_INFO(("redisConnectUnix slave redis ip[%s] %s",g_ctx.config[REDIS_SLAVE_IP],redisctx->errstr));
                return NULL;
            }
        }
        else
        {
            redisctx= redisConnectWithTimeout((char*)(g_ctx.config[REDIS_SLAVE_IP]), port, timeout);
            if (redisctx->err) {  
                DEBUG_INFO(("redisConnect slave redis ip[%s] %s",g_ctx.config[REDIS_IP],redisctx->errstr));

                return NULL;
            }
        }

        if (cliAuth(redisctx) != REDIS_OK)
            return NULL;
    }
    else
    {
        redisctx= get_redis_conn();
    }

    redisctx->flags |= REDIS_CONNECTED;
    return redisctx;
}

int db_conn_init(int conn_num)
{
    int i;
    int redis_num;
    int index;
    struct redisContext* r = NULL;
    char *port_arr[conn_num + 1];

    redis_num = -1;
    while ((port_arr[++redis_num] = strsep(&g_ctx.config[REDIS_SLAVE_IP_PORT], ",")));

	g_sql_conn = (sql_conn_t *)malloc(sizeof(sql_conn_t) * conn_num);
    if(!g_sql_conn) return -1;

    g_redis_conn = (redisContext **)malloc(sizeof(struct redisContext*) * conn_num);
    if (!g_redis_conn) return -1;

    g_rredis_conn = (redisContext **)malloc(sizeof(struct redisContext*) * conn_num);
    if (!g_rredis_conn) return -1;

    g_sredis_conn = (redisContext **)malloc(sizeof(struct redisContext*) * conn_num);
    if (!g_sredis_conn) return -1;

    for( i = 0; i < conn_num; ++i)
    {
        if(!(g_redis_conn[i] = get_redis_conn())) return -1;

        if(!(g_rredis_conn[i] = get_redis_conn())) return -1;

        index = i % redis_num;
        if(!index) index = redis_num - 1;
        if(!(g_sredis_conn[i] = get_sredis_conn(atoi(port_arr[index])))) return -1;

        g_sql_conn[i].db_proc = NULL;
        g_sql_conn[i].create_time = 0;
    }

    if(!(r = get_sredis_conn(atoi(port_arr[0])))) return -1;
    g_log_redis_conn = r;

    return 0;
}

void db_conn_destroy(int conn_num)
{
    int i;
    for(i = 0; i < conn_num; ++i)
    {
        redisFree(g_redis_conn[i]);
        redisFree(g_rredis_conn[i]);
        redisFree(g_sredis_conn[i]);
        if(g_sql_conn[i].db_proc) dbclose(g_sql_conn[i].db_proc);
    }
    redisFree(g_log_redis_conn);
    free(g_redis_conn);
    free(g_rredis_conn);
    free(g_sredis_conn);
    free(g_sql_conn);
    g_redis_conn = NULL;
    g_sredis_conn = NULL;
    g_sql_conn = NULL;
}

int hash_init(int num)
{
    int i, j;

    g_ht_node_pools = (ht_node_pool_t*)malloc(sizeof(ht_node_pool_t) * num);
    if(!g_ht_node_pools) return -1;

    g_ht_map_pools = (ht_map_pool_t*)malloc(sizeof(ht_map_pool_t) * num);
    if(!g_ht_map_pools)
    {
        free(g_ht_node_pools);

        return -1;
    }

    for(i = 0; i < num; ++i)
    {
        init_ht_node_pool(&g_ht_node_pools[i], NODE_POOL_SIZE);

        if(!g_ht_node_pools[i].size) return -1;

        init_ht_map_pool(&g_ht_map_pools[i], MAP_POOL_SIZE);

        if(!g_ht_map_pools[i].size) return -1;

        for(j = 0; j < MAP_POOL_SIZE; ++j)
        {
            g_ht_map_pools[i].ht_arr[j] = eg_ht_create(AD_SIZE, NULL, NULL, NULL, NULL);
        }
    }

    return 0;
}

void hash_destroy(int num)
{
    int i, j;

    for(i = 0; i < num; ++i)
    {
        destroy_ht_node_pool(&g_ht_node_pools[i], NODE_POOL_SIZE);

        for(j = 0; j < MAP_POOL_SIZE; ++j)
        {
            ht_clear_bucket(g_ht_map_pools[i].ht_arr[j]);
            eg_ht_destroy(g_ht_map_pools[i].ht_arr[j]);
        }
        destroy_ht_map_pool(&g_ht_map_pools[i], MAP_POOL_SIZE);
    }
    free(g_ht_node_pools);
    free(g_ht_map_pools);
    g_ht_node_pools = NULL;
    g_ht_map_pools = NULL;
}

void log_cq_destroy(int num)
{
    int i;

    for(i = 0; i < num; ++i)
    {
        eg_cq_destroy(g_log_queues[i]);
    }
    free(g_log_queues);
}

int log_cq_init(int num)
{
    int i;

    g_log_queues = (eg_cycle_queue_t **)malloc(sizeof(eg_cycle_queue_t *) * num);
    if(!g_log_queues) return -1;

    for(i = 0; i < num; ++i)
    {
       g_log_queues[i] =  eg_cq_create(LOG_QUEUE_LEN, LOG_CONTENT_LEN);
       if(!g_log_queues[i])
       {
           log_cq_destroy(i);

           return -1;
       }
    }

    return 0;
}

int create_conn(cache_pool_t *cp)
{
    int i;
    int len;
    void *buf;
    conn *c;
    struct ais_connection *ac;
    int size = cp->head->size;
    int tid = cp->tid;

    buf = calloc(size, sizeof(conn)+ DATA_BUFFER_SIZE + DATA_BUFFER_SIZE 
            + IOV_LIST_INITIAL *sizeof(struct iovec) + MSG_LIST_INITIAL * sizeof(struct msghdr) 
            + sizeof(struct ais_connection) + MAX_REQUEST_BUF_SIZE);
    if(!buf) return 0;

    cp->head->buf = buf; 
    for(i = 0; i < size; ++i)
    {
        len = 0;
        c = (conn *)buf;
        c->rbuf = c->wbuf = 0;
        c->iov = 0;
        c->msglist = 0;
        c->hdrbuf = 0;
        c->rsize = DATA_BUFFER_SIZE;
        c->wsize = DATA_BUFFER_SIZE;
        c->iovsize = IOV_LIST_INITIAL;
        c->msgsize = MSG_LIST_INITIAL;
        c->hdrsize = 0;

        len = sizeof(conn);
        c->rbuf = (char *)(buf + len);
        len += DATA_BUFFER_SIZE;
        c->wbuf = (char *)(buf + len);
        len += DATA_BUFFER_SIZE;
        c->iov = (struct iovec *)(buf + len);
        len += IOV_LIST_INITIAL *sizeof(struct iovec);
        c->msglist = (struct msghdr *)(buf + len);	
        
        len += MSG_LIST_INITIAL * sizeof(struct msghdr);
	    ac =  (struct ais_connection *)(buf + len);
	    ac->ctx = &g_ctx;
	    ac->redisctx = g_redis_conn[tid];
	    ac->rredisctx = g_rredis_conn[tid];
	    ac->sredisctx = g_sredis_conn[tid];
	    ac->node_pool = &g_ht_node_pools[tid];
	    ac->map_pool = &g_ht_map_pools[tid];
        ac->cq = g_log_queues[tid];
        c->bulk = ac;
        len += sizeof(struct ais_connection) + MAX_REQUEST_BUF_SIZE;
        if(i != size - 1) buf += len;
        eg_cq_push(cp->cq, c);
    }

    return 1;
}

int create_conn_item(cache_pool_t *cp)
{
    int i;
    void *buf;
    int size = cp->head->size;

    buf = calloc(size, sizeof(CQ_ITEM));
    if(!buf) return 0;

    cp->head->buf = buf; 
    for(i = 0; i < size; ++i)
    {
        eg_cq_push(cp->cq, buf);
        if(i != size - 1) buf += sizeof(CQ_ITEM) * i;
    }

    return 1;
}

cache_pool_t *cache_pool_create(int size, int (*create_obj)(struct cache_pool *cp), int tid)
{
    cache_pool_t *cp;
    pool_bucket_t *bt;

    cp= (cache_pool_t *)malloc(sizeof(cache_pool_t));
    if(!cp) return NULL;

    cp->size = size;
    cp->tid = tid;
    bt = (pool_bucket_t *)malloc(sizeof(pool_bucket_t));
    if(!bt)
    {
        free(cp);

        return NULL;
    }
    bt->size = size;
    cp->head = bt;
    
    cp->head->next = NULL;
    cp->cq = eg_cq_create(size + 1, 0);
    if(!cp->cq)
    {
        free(cp);
        free(bt);

        return NULL;
    }
    cp->create_obj = create_obj;

    if(cp->create_obj(cp)) return cp;

    eg_cq_destroy(cp->cq);
    free(cp);
    free(bt);

    return NULL;
}

void cache_pool_destroy(cache_pool_t *cp)
{
    if(!cp) return;

    pool_bucket_t *bt;
    pool_bucket_t *tmp;

    eg_cq_destroy(cp->cq);
    bt = cp->head;
    while(bt)
    {
        tmp = bt;
        bt = bt->next;
        free(tmp->buf);
        free(tmp);
    }
    free(cp);
}

void *cp_pop(cache_pool_t *cp)
{
    if(!cp) return NULL;

    void *c = eg_cq_pop(cp->cq);
    if(c) return c;

    pool_bucket_t *bt;
    bt = (pool_bucket_t *)malloc(sizeof(pool_bucket_t));

    if(!bt) return NULL;

    bt->size = cp->head->size;
    bt->next = cp->head;
    cp->head = bt;
    if(cp->create_obj(cp)) return eg_cq_pop(cp->cq);

    return NULL;
}

int cp_push(cache_pool_t *cp, void *c)
{
    if(!cp) return 0;

    return eg_cq_push(cp->cq, c);
}

int conn_pools_init(int num)
{
    int i;

    g_conn_pools = (cache_pool_t **)malloc(sizeof(cache_pool_t *) * num);
    if(!g_conn_pools) return -1;

    g_conn_item_queues = (eg_cycle_queue_t **)malloc(sizeof(eg_cycle_queue_t *) * num);
    if(!g_conn_item_queues) return -1;

    for(i = 0; i < num; ++i)
    {
        g_conn_pools[i] = cache_pool_create(CONN_QUEUE_LEN, create_conn, i);
        if(!g_conn_pools[i]) return -1;

        g_conn_item_queues[i] =  eg_cq_create(CONN_ITEM_LEN, sizeof(CQ_ITEM));
        if(!g_conn_item_queues[i]) return -1;
    }

    return 0;
}

void conn_pools_destroy(int num)
{
    int i;

    for(i = 0; i < num; ++i)
    {
        cache_pool_destroy(g_conn_pools[i]);
        eg_cq_destroy(g_conn_item_queues[i]);
    }
    free(g_conn_pools);
    free(g_conn_item_queues);
}

static void sig_handler(const int sig) 
{
    g_log_empty = 0;
    sem_post(&g_log_sem);
	do_run_maintenance_thread = 0;
	pthread_join(maintenance_tid, NULL);

    DEBUG_INFO(("SIGINT handled.---%d\n",sig));
    signal(SIGSEGV, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);

    raise(sig);
 }

#ifndef HAVE_SIGIGNORE
static int sigignore(int sig) {
    struct sigaction sa = { .sa_handler = SIG_IGN, .sa_flags = 0 };

    if (sigemptyset(&sa.sa_mask) == -1 || sigaction(sig, &sa, 0) == -1) {
        return -1;
    }
    return 0;
}
#endif

/*
 * On systems that supports multiple page sizes we may reduce the
 * number of TLB-misses by using the biggest available page size
 */
static int enable_large_pages(void) {
#if defined(HAVE_GETPAGESIZES) && defined(HAVE_MEMCNTL)
    int ret = -1;
    size_t sizes[32];
    int avail = getpagesizes(sizes, 32);
    if (avail != -1) {
        size_t max = sizes[0];
        struct memcntl_mha arg = {0};
        int ii;

        for (ii = 1; ii < avail; ++ii) {
            if (max < sizes[ii]) {
                max = sizes[ii];
            }
        }

        arg.mha_flags   = 0;
        arg.mha_pagesize = max;
        arg.mha_cmd = MHA_MAPSIZE_BSSBRK;

        if (memcntl(0, 0, MC_HAT_ADVISE, (caddr_t)&arg, 0, 0) == -1) {
            fprintf(stderr, "Failed to set large pages: %s\n",
                    strerror(errno));
            fprintf(stderr, "Will use default page size\n");
        } else {
            ret = 0;
        }
    } else {
        fprintf(stderr, "Failed to get supported pagesizes: %s\n",
                strerror(errno));
        fprintf(stderr, "Will use default page size\n");
    }

    return ret;
#else
    return 0;
#endif
}

/**
 * Do basic sanity check of the runtime environment
 * @return true if no errors found, false if we can't use this env
 */
static bool sanitycheck(void) {
    /* One of our biggest problems is old and bogus libevents */
    const char *ever = event_get_version();
    if (ever != NULL) {
        if (strncmp(ever, "1.", 2) == 0) {
            /* Require at least 1.3 (that's still a couple of years old) */
            if ((ever[2] == '1' || ever[2] == '2') && !isdigit(ever[3])) {
                fprintf(stderr, "You are using libevent %s.\nPlease upgrade to"
                        " a more recent version (1.3 or newer)\n",
                        event_get_version());
                return false;
            }
        }
    }

    return true;
}
#ifdef TXT_LOG




#ifdef TXT
static void my_localtime(time_t now, struct tm *ret)
{
    struct tm  *t;

    t = localtime_r(&now,t);
    *ret = *t;

    ret->tm_mon++;
    ret->tm_year += 1900;

    return;
}

static void *log_work_thread(void * arg)
{
	(void)arg;
	char filename[512] = {0};
	struct tm current;
	time_t now = time(NULL);
	time_t bak_now;
	int switch_time = atoi(g_ctx.config[LOG_SWITCH_TIME]);
	my_localtime(now, &current);
	snprintf(filename, sizeof(filename), "%s/ais_%04d%02d%02d_%02d%02d%02d_%s.log", g_ctx.config[LOG_WORK_PATH], 
	current.tm_year, current.tm_mon, current.tm_mday, current.tm_hour, current.tm_min, current.tm_sec,g_ctx.config[LISTENING_PORTS] );
	record_fp = fopen(filename, "a+");
	if(record_fp == 0)
	{
		fprintf(stderr,  "%s[%d]: open file %s error: %s\n", __func__, __LINE__, filename, strerror(errno));
		return NULL;
	}
	else
	{
		fprintf(stderr,  "%s[%d]: file[%s]\n", __func__, __LINE__, filename);
	}

	// work cycle
	while(do_run_maintenance_thread)
	{
		bak_now = time(NULL);
		if(bak_now - now >= switch_time)
		{        
			fflush(record_fp);
			fclose(record_fp);
		
			// rename and reopen
			now = bak_now;
			my_localtime(now, &current);
			snprintf(filename, sizeof(filename), "%s/ais_%04d%02d%02d_%02d%02d%02d_%s.log",g_ctx.config[LOG_WORK_PATH],
			current.tm_year, current.tm_mon, current.tm_mday, current.tm_hour, current.tm_min, current.tm_sec,g_ctx.config[LISTENING_PORTS] );
			record_fp = fopen(filename, "a+");
			if(record_fp == 0)
			{
				fprintf(stderr,"%s[%d]: open file %s error: %s", __func__, __LINE__, filename, strerror(errno));
				return NULL;
			}
			else
			{
				fprintf(stderr,  "%s[%d]: file[%s]\n", __func__, __LINE__, filename);
			}

		
		}
		
		DEBUG_TRACE(("redis_log_list length =%d",x_list_length(redis_log_list)));
		struct loginfo_packet_t *lpr = x_list_pop(redis_log_list, true);
		if(lpr == NULL)
		{
			fprintf(stderr,  "%s[%d]:  x_list_pop return null\n", __func__, __LINE__);
		}
		
			
			fprintf(record_fp, "%s\n",lpr->buf);
			if(lpr->buf)
			free(lpr->buf);
			free(lpr);
			//fflush(record_fp);//FIXME
		
		
	}
	fprintf(stderr,  "%s[%d] log_work_thread finish \n", __func__, __LINE__);
	return NULL;
}
#endif


#ifdef REDIS

static void *log_work_thread(void * arg)
{
    int i;
    int j;
    int num;
    int thread_num = (unsigned long)arg;	
    struct timespec ts;
    char *str;
    struct redisContext *r = g_log_redis_conn;
    eg_cycle_queue_t **log_queues;
    eg_cycle_queue_t *tmp;

    log_queues = (eg_cycle_queue_t **)malloc(sizeof(eg_cycle_queue_t *) * thread_num);
    for(i = 0; i < thread_num; i++)
    {
        log_queues[i] = g_log_queues[i];
    }

    while (do_run_maintenance_thread || g_log_empty <= 1)
    {
        if(1 == g_log_empty)
        {
            ts.tv_sec = time(NULL) + 1;
            ts.tv_nsec = 0;
            while (sem_timedwait(&g_log_sem, &ts) == -1 && errno == EINTR);
        }else
        {
            while (sem_wait(&g_log_sem) == -1 && errno == EINTR);
        }

        do{
            num = thread_num;
            for(i = 0; i < num; i++)
            {
                for(j = 0; j < 100; ++j)
                {
                    str = (char *)eg_cq_head(log_queues[i]);
                    if(!str) break;

                    redis_write_log(r, "LPUSH log:info  %s", str); 
                    eg_cq_pop(log_queues[i]);
                }

                if(j != 100)
                {
                    tmp = log_queues[i];
                    log_queues[i--] = log_queues[--num];
                    log_queues[num] = tmp;
                }
            }
        }while (num);

        ++g_log_empty;
    }
    
    free(log_queues);
    DEBUG_INFO(("%s[%d] log_work_thread finish \n", __func__, __LINE__));

    return NULL;
}
#endif

#endif
DBPROCESS *db_g_proc;

int main (int argc, char **argv) {
    int c;
    bool lock_memory = false;
    bool do_daemonize = false;
    bool preallocate = false;
    int maxcore = 0;
    char *username = NULL;
    char *pid_file = NULL;
    struct passwd *pw;
    struct rlimit rlim;
    /* listening sockets */
    static int *l_socket = NULL;
    #ifdef TXT_LOG
    int ret;
    #endif
    /* udp socket */
    static int *u_socket = NULL;
    bool protocol_specified = false;
    bool tcp_specified = false;
    bool udp_specified = false;
	FILE *fp = NULL;
    char *subopts;
    char *subopts_value;
    enum {
        MAXCONNS_FAST = 0,
        HASHPOWER_INIT
    };
    /*
    char *const subopts_tokens[] = {
        [MAXCONNS_FAST] = "maxconns_fast",
        [HASHPOWER_INIT] = "hashpower",
        NULL
    };*/
     char *const subopts_tokens[] = {
        "maxconns_fast",
        "hashpower",
		NULL
    };

    if (!sanitycheck()) {
        return EX_OSERR;
    }

    /* handle SIGINT */
    signal(SIGINT, sig_handler);
    signal(SIGSEGV, sig_handler);
    signal(SIGQUIT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGKILL, sig_handler);
    signal(SIGPIPE, SIG_IGN);

    /* init settings */
    settings_init();

    /* set stderr non-buffering (for running under, say, daemontools) */
    setbuf(stderr, NULL);

    /* process arguments */
    while (-1 != (c = getopt(argc, argv,
          "a:"  /* access mask for unix socket */
          "p:"  /* TCP port number to listen on */
          "s:"  /* unix socket path to listen on */
          "c:"  /* max simultaneous connections */
          "k"   /* lock down all paged memory */
          "r"   /* maximize core file limit */
          "v"   /* verbose */
          "d"   /* daemon mode */
          "l:"  /* interface to listen on */
          "u:"  /* user identity to run as */
          "P:"  /* save PID in file */
          "L"   /* Large memory pages */
          "R:"  /* max requests per event */
          "C:"   /* Disable use of CAS */
          "b:"  /* backlog queue limit */
          "B:"  /* Binding protocol */
          "o:"  /* Extended generic options */
          "S:"  ///sqlserver connect  string
          "U:"  //sqlserver user
          "W:"  //sqlserver passwrd
          "D:"  //dbname
        ))) {
        switch (c) {
        case 'a':
            /* access for unix domain socket, as octal mask (like chmod)*/
            settings.access= strtol(optarg,NULL,8);
            break;


        case 'p':
            settings.port = atoi(optarg);
            tcp_specified = true;
            break;
        case 's':
            settings.socketpath = optarg;
            break;


        case 'c':
            settings.maxconns = atoi(optarg);
            break;

        case 'k':
            lock_memory = true;
            break;
        case 'v':
            settings.verbose++;
            break;
        case 'l':
            if (settings.inter != NULL) {
                size_t len = strlen(settings.inter) + strlen(optarg) + 2;
                char *p = (char *)malloc(len);
                if (p == NULL) {
                    fprintf(stderr, "Failed to allocate memory\n");
                    return 1;
                }
                snprintf(p, len, "%s,%s", settings.inter, optarg);
                free(settings.inter);
                settings.inter = p;
            } else {
                settings.inter= strdup(optarg);
            }
            break;
        case 'd':
            do_daemonize = true;
            break;
        case 'r':
            maxcore = 1;
            break;
	    case 'S':
			strcpy(settings.conn_str,optarg);
			break;
		case 'U':
			strcpy(settings.sql_user_name,optarg);
			break;
	    case 'W':
			strcpy(settings.sql_user_pwd,optarg);
			break;
	    case 'D':
			strcpy(settings.db_name,optarg);
		    break;
        case 'R':
            settings.reqs_per_event = atoi(optarg);
            if (settings.reqs_per_event == 0) {
                fprintf(stderr, "Number of requests per event must be greater than 0\n");
                return 1;
            }
            break;
        case 'u':
            username = optarg;
            break;
        case 'P':
            pid_file = optarg;
            break;
       
        case 'L' :
            if (enable_large_pages() == 0) {
                preallocate = true;
            }
            break;
        case 'C' :
			memcpy(config_file,optarg,strlen(optarg));
            break;
        case 'b' :
            settings.backlog = atoi(optarg);
            break;
        case 'B':
            protocol_specified = true;
            if (strcmp(optarg, "auto") == 0) {
                settings.binding_protocol = negotiating_prot;
            } else if (strcmp(optarg, "binary") == 0) {
                settings.binding_protocol = binary_prot;
            } else if (strcmp(optarg, "ascii") == 0) {
                settings.binding_protocol = ascii_prot;
            } else {
                fprintf(stderr, "Invalid value for binding protocol: %s\n"
                        " -- should be one of auto, binary, or ascii\n", optarg);
                exit(EX_USAGE);
            }
            break;


        case 'o': /* It's sub-opts time! */
            subopts = optarg;

            while (*subopts != '\0') {

            switch (getsubopt(&subopts, subopts_tokens, &subopts_value)) {
            case MAXCONNS_FAST:
                settings.maxconns_fast = true;
                break;
            case HASHPOWER_INIT:
                if (subopts_value == NULL) {
                    fprintf(stderr, "Missing numeric argument for hashpower\n");
                    return 1;
                }
                settings.hashpower_init = atoi(subopts_value);
                if (settings.hashpower_init < 12) {
                    fprintf(stderr, "Initial hashtable multiplier of %d is too low\n",
                        settings.hashpower_init);
                    return 1;
                } else if (settings.hashpower_init > 64) {
                    fprintf(stderr, "Initial hashtable multiplier of %d is too high\n"
                        "Choose a value based on \"STAT hash_power_level\" from a running instance\n",
                        settings.hashpower_init);
                    return 1;
                }
                break;
            default:
                printf("Illegal suboption \"%s\"\n", subopts_value);
                return 1;
            }

            }
            break;
        default:
            fprintf(stderr, "Illegal argument \"%c\"\n", c);
            return 1;
        }
    }
	if(username == NULL) username = "root";
	setting_init2();
/*	if(db_connect(settings.sql_user_name,settings.sql_user_pwd,settings.conn_str,settings.db_name,&db_g_proc)==FAIL)
	   	{
	   	  fprintf(stderr,"connet to sql_server:%s user=%s error\n",settings.conn_str,settings.sql_user_name);	
		  return 0;
	   	}
	  dbclose(db_g_proc); */
    /*
     * Use one workerthread to serve each UDP port if the user specified
     * multiple ports
     */
    if (settings.inter != NULL && strchr(settings.inter, ',')) {
        settings.num_threads_per_udp = 1;
    } else {
        settings.num_threads_per_udp = settings.num_threads;
    }

    if (settings.sasl) {
        if (!protocol_specified) {
            settings.binding_protocol = binary_prot;
        } else {
            if (settings.binding_protocol != binary_prot) {
                fprintf(stderr, "ERROR: You cannot allow the ASCII protocol while using SASL.\n");
                exit(EX_USAGE);
            }
        }
    }

    if (tcp_specified && !udp_specified) {
        settings.udpport = settings.port;
    } else if (udp_specified && !tcp_specified) {
        settings.port = settings.udpport;
    }

    if (maxcore != 0) {
        struct rlimit rlim_new;
        /*
         * First try raising to infinity; if that fails, try bringing
         * the soft limit to the hard.
         */
        if (getrlimit(RLIMIT_CORE, &rlim) == 0) {
            rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
            if (setrlimit(RLIMIT_CORE, &rlim_new)!= 0) {
                /* failed. try raising just to the old max */
                rlim_new.rlim_cur = rlim_new.rlim_max = rlim.rlim_max;
                (void)setrlimit(RLIMIT_CORE, &rlim_new);
            }
        }
        /*
         * getrlimit again to see what we ended up with. Only fail if
         * the soft limit ends up 0, because then no core files will be
         * created at all.
         */

        if ((getrlimit(RLIMIT_CORE, &rlim) != 0) || rlim.rlim_cur == 0) {
            fprintf(stderr, "failed to ensure corefile creation\n");
            exit(EX_OSERR);
        }
    }

    /*
     * If needed, increase rlimits to allow as many connections
     * as needed.
     */

    if (getrlimit(RLIMIT_NOFILE, &rlim) != 0) {
        fprintf(stderr, "failed to getrlimit number of files\n");
        exit(EX_OSERR);
    } else {
        rlim.rlim_cur = settings.maxconns;
        if (setrlimit(RLIMIT_NOFILE, &rlim) != 0) {
            fprintf(stderr, "failed to set rlimit for open files. Try starting as root or requesting smaller maxconns value.\n");
            exit(EX_OSERR);
        }
    }

    /* lose root privileges if we have them */
    if (getuid() == 0 || geteuid() == 0) {
        if (username == 0 || *username == '\0') {
            fprintf(stderr, "can't run as root without the -u switch\n");
            exit(EX_USAGE);
        }
        if ((pw = getpwnam(username)) == 0) {
            fprintf(stderr, "can't find the user %s to switch to\n", username);
            exit(EX_NOUSER);
        }
        if (setgid(pw->pw_gid) < 0 || setuid(pw->pw_uid) < 0) {
            fprintf(stderr, "failed to assume identity of user %s\n", username);
            exit(EX_OSERR);
        }
    }


    /* daemonize if requested */
    /* if we want to ensure our ability to dump core, don't chdir to / */
    if (do_daemonize) {
        if (sigignore(SIGHUP) == -1) {
            perror("Failed to ignore SIGHUP");
        }
        if (daemonize(maxcore, settings.verbose) == -1) {
            fprintf(stderr, "failed to daemon() in order to daemonize\n");
            exit(EXIT_FAILURE);
        }
        if(g_ctx.config[DEBUG_LOG_FILE])
        {
            fp = fopen( g_ctx.config[DEBUG_LOG_FILE], "a+" );
            if (fp == NULL)
            {
                fprintf(stderr, "debug_log_file creat failed\n");
                exit(EXIT_FAILURE);
            }
            if( dup2(fileno(fp), STDERR_FILENO) == -1 )
            {
                fprintf(stderr, "dup2 STDERR_FILENO  failed\n");
                exit(EXIT_FAILURE);
            }
            if( dup2(fileno(fp), STDOUT_FILENO) == -1 )
            {
                fprintf(stderr, "dup2 STDOUT_FILENO  failed\n");
                exit(EXIT_FAILURE);
            }
            //fclose(fp);

        }
    }

    /* lock paged memory if needed */
    if (lock_memory) {
#ifdef HAVE_MLOCKALL
        int res = mlockall(MCL_CURRENT | MCL_FUTURE);
        if (res != 0) {
            fprintf(stderr, "warning: -k invalid, mlockall() failed: %s\n",
                    strerror(errno));
        }
#else
        fprintf(stderr, "warning: -k invalid, mlockall() not supported on this platform.  proceeding without.\n");
#endif
    }
    if (dbinit() == FAIL)
    {
        perror("failed to init sybdb\n");
        exit(EX_OSERR);
    }

    /* initialize main thread libevent instance */
    main_base = event_init();

    /* initialize other stuff */
    stats_init();

    /*
     * ignore SIGPIPE signals; we can use errno == EPIPE if we
     * need that information
     */
    if (sigignore(SIGPIPE) == -1) {
        perror("failed to ignore SIGPIPE; sigaction\n");
        exit(EX_OSERR);
    }
    /* start up worker threads if MT mode */
    if(db_conn_init(settings.num_threads + 1) == -1)
    {
        perror("failed to connect redis\n");

        exit(EX_OSERR);
    }
    if(hash_init(settings.num_threads + 1) == -1)
    {
        perror("create hash node failed\n");

        exit(EX_OSERR);
    }
    thread_init(settings.num_threads, main_base);

#ifdef TXT_LOG
    g_log_empty = 1;
    if (sem_init(&g_log_sem, 0, 0) == -1)
    {
        perror("Can't init log sem\n");
    }
    if (log_cq_init(settings.num_threads + 1) == -1)
    {
        perror("create log cycle queue failed\n");

        exit(EX_OSERR);
    }
    if (conn_pools_init(settings.num_threads + 1) == -1)
    {
        perror("create conn pools failed\n");

        exit(EX_OSERR);
    }
    if ((ret = pthread_create(&maintenance_tid, NULL,
                    log_work_thread, (void *)(settings.num_threads + 1UL))) != 0) {
        fprintf(stderr, "Can't create thread: %s\n", strerror(ret));
        exit(EX_OSERR);
    }
#endif
    /* initialise clock event */
    clock_handler(0, 0, 0);

    /* create unix mode sockets after dropping privileges */
    if (settings.socketpath != NULL) {
        errno = 0;
        if (server_socket_unix(settings.socketpath,settings.access, settings.num_threads)) {
            vperror("failed to listen on UNIX socket: %s", settings.socketpath);
            exit(EX_OSERR);
        }
    }

    /* create the listening socket, bind it, and init */
    if (settings.socketpath == NULL) {
        const char *portnumber_filename = getenv("AIS_PORT_FILENAME");
        char temp_portnumber_filename[PATH_MAX];
        FILE *portnumber_file = NULL;

        if (portnumber_filename != NULL) {
            snprintf(temp_portnumber_filename,
                     sizeof(temp_portnumber_filename),
                     "%s.lck", portnumber_filename);

            portnumber_file = fopen(temp_portnumber_filename, "a");
            if (portnumber_file == NULL) {
                fprintf(stderr, "Failed to open \"%s\": %s\n",
                        temp_portnumber_filename, strerror(errno));
            }
        }

        errno = 0;
        if (settings.port && server_sockets(settings.port, tcp_transport,
                                           portnumber_file, settings.num_threads)) {
            vperror("failed to listen on TCP port %d", settings.port);
            exit(EX_OSERR);
        }

        /*
         * initialization order: first create the listening sockets
         * (may need root on low ports), then drop root if needed,
         * then daemonise if needed, then init libevent (in some cases
         * descriptors created by libevent wouldn't survive forking).
         */

        /* create the UDP listening socket and bind it */
        errno = 0;
        if (settings.udpport && server_sockets(settings.udpport, udp_transport,
                                              portnumber_file, settings.num_threads)) {
            vperror("failed to listen on UDP port %d", settings.udpport);
            exit(EX_OSERR);
        }

        if (portnumber_file) {
            fclose(portnumber_file);
            rename(temp_portnumber_filename, portnumber_filename);
        }
    }


	/*init adsdk*/
	if(init_adsdk() != 0)
	{	
		fprintf(stderr, "init_adsdk failed.\n");
		exit(EX_OSERR);
	}

    /* Give the sockets a moment to open. I know this is dumb, but the error
     * is only an advisory.
     */
    usleep(1000);
    if (stats.curr_conns + stats.reserved_fds >= settings.maxconns - 1) {
        fprintf(stderr, "Maxconns setting is too low, use -c to increase.\n");
        exit(EXIT_FAILURE);
    }

    if (pid_file != NULL) {
        save_pid(pid_file);
    }

    /* enter the event loop */
    event_base_loop(main_base, 0);
    event_base_free(main_base); 
    exit_threads();
#ifdef TXT_LOG
    g_log_empty = 0;
    sem_post(&g_log_sem);
	do_run_maintenance_thread = 0;
	pthread_join(maintenance_tid, NULL);
    log_cq_destroy(settings.num_threads + 1);
    sem_destroy(&g_log_sem);
#endif
    db_conn_destroy(settings.num_threads + 1);
    hash_destroy(settings.num_threads + 1);
    conn_pools_destroy(settings.num_threads + 1);
    settings_destroy();
    /* remove the PID file if we're a daemon */
    if (do_daemonize) remove_pidfile(pid_file);
    /* Clean up strdup() call for bind() address */
    if (settings.inter) free(settings.inter);
    if (l_socket) free(l_socket);
    if (u_socket) free(u_socket);
    if(fp) fclose(fp);
    dbexit();

    return EXIT_SUCCESS;
}
