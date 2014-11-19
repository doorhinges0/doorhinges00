#include <errno.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>       
#include <netinet/tcp.h>      
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "redis_keepalive.h"
#include "ais.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "redisc/net.h"
#include "redisc/sds.h"
#ifdef __cplusplus
}
#endif // __cplusplus


namespace rediskeepalive{
//Async connect
redisContext *__AsyncConnect(redisContext *c, const char *ip, int port) {
    c->flags &= ~REDIS_BLOCK;
    __ContextConnectTcp(c,ip,port,NULL);
}

int __CheckSocketError(redisContext *c, int fd, int &err) {return 0;}

int __AsyncHandleConnect(redisContext *c, int &err) {
    err=0;
    socklen_t errlen = sizeof(err);

    fd_set rset, wset;
    struct timeval tval;

    FD_ZERO(&rset);
    FD_ZERO(&wset);
    FD_SET(c->fd, &rset);
    wset=rset;
    tval.tv_sec=1;
    tval.tv_usec=0;

    int n=0;
    if (0==(n=select(c->fd+1, &rset, &wset, NULL, &tval)))
    {//timeout
        return REDIS_ERR;
    }

    fprintf(stderr, "n:%d, c->fd:%d\n", n, c->fd);
    if ( FD_ISSET(c->fd, &rset) || FD_ISSET(c->fd, &wset))
    {/*#define ECONNREFUSED    //111  Connection refused */
        if (getsockopt(c->fd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
            __SetErrorFromErrno(c,REDIS_ERR_IO,"getsockopt(SO_ERROR)");
            close(c->fd);
            c->fd=-1;
            return REDIS_ERR;
        }
        
        fprintf(stderr, "getsockopt(), err:%d\n", err);
        if (err) {
            __SetErrorFromErrno(c,REDIS_ERR_IO,NULL);
            close(c->fd);
            c->fd=-1;
            return REDIS_ERR;
        }

        /* Mark context as connected. */
        __onConnect(c);
        return REDIS_OK;
    }

    return REDIS_OK;
}

int __onConnect(redisContext *c)
{
    c->flags |= REDIS_CONNECTED;
    __redisContextInit(c);
}

void __redisContextInit(redisContext *c)
{
    c->err = 0;
    c->errstr[0] = '\0';
    c->obuf = sdsempty();
    c->reader = redisReaderCreate();
}

int __disConnect(redisContext *c)
{
    c->flags &= ~REDIS_CONNECTED;

    __releaseResource(c);
}

void __releaseResource(redisContext *c)
{
    if (c->fd > 0){
        close(c->fd);
        c->fd=-1;
    }
    if (c->obuf != NULL){
        sdsfree(c->obuf);
        c->obuf = NULL;
    }
    if (c->reader != NULL){
        redisReaderFree(c->reader);
        c->reader = NULL;
    }
}

void __SetErrorFromErrno(redisContext *c, int type, const char *prefix) {
    char buf[128];
    size_t len = 0;

    if (prefix != NULL)
        len = snprintf(buf,sizeof(buf),"%s: ",prefix);
    strerror_r(errno,buf+len,sizeof(buf)-len);
    __SetError(c,type,buf);
}

void __SetError(redisContext *c, int type, const char *str) {
    size_t len;

    c->err = type;
    if (str != NULL) {
        len = strlen(str);
        len = len < (sizeof(c->errstr)-1) ? len : (sizeof(c->errstr)-1);
        memcpy(c->errstr,str,len);
        c->errstr[len] = '\0';
    } else {
        /* Only REDIS_ERR_IO may lack a description! */
        if (type == REDIS_ERR_IO){
            strerror_r(errno,c->errstr,sizeof(c->errstr));
        }
    }
}

int __ContextConnectTcp(redisContext *c, const char *addr, int port, struct timeval *timeout) {
    int s, rv;
    char _port[6];  /* strlen("65535"); */
    struct addrinfo hints, *servinfo, *p;
    int blocking = (c->flags & REDIS_BLOCK);

    snprintf(_port, 6, "%d", port);
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(addr,_port,&hints,&servinfo)) != 0) {
        __SetError(c,REDIS_ERR_OTHER,gai_strerror(rv));
        return REDIS_ERR;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((s = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
            continue;

        if (__SetBlocking(c,s,0) != REDIS_OK)
            goto error;
        if (connect(s,p->ai_addr,p->ai_addrlen) == -1) {
            if (errno == EHOSTUNREACH) {
                close(s);
                continue;
            } else if (errno == EINPROGRESS && !blocking) {
                /* This is ok. */
            }
        }

        if (__SetTcpNoDelay(c,s) != REDIS_OK)
            goto error;

        c->fd = s;
        rv = REDIS_OK;
        goto end;
    }
    if (p == NULL) {
        char buf[128];
        snprintf(buf,sizeof(buf),"Can't create socket: %s",strerror(errno));
        __SetError(c,REDIS_ERR_OTHER,buf);
        goto error;
    }

error:
    rv = REDIS_ERR;
end:
    freeaddrinfo(servinfo);
    return rv;  // Need to return REDIS_OK if alright
}

int __SetBlocking(redisContext *c, int fd, int blocking) {
    int flags;

    /* Set the socket nonblocking.
     * Note that fcntl(2) for F_GETFL and F_SETFL can't be
     * interrupted by a signal. */
    if ((flags = fcntl(fd, F_GETFL)) == -1) {
        __SetErrorFromErrno(c,REDIS_ERR_IO,"fcntl(F_GETFL)");
        close(fd);
        return REDIS_ERR;
    }

    if (blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;

    if (fcntl(fd, F_SETFL, flags) == -1) {
        __SetErrorFromErrno(c,REDIS_ERR_IO,"fcntl(F_SETFL)");
        close(fd);
        return REDIS_ERR;
    }
    return REDIS_OK;
}

int __SetTcpNoDelay(redisContext *c, int fd) {
    int yes = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1) {
        __SetErrorFromErrno(c,REDIS_ERR_IO,"setsockopt(TCP_NODELAY)");
        close(fd);
        return REDIS_ERR;
    }
    return REDIS_OK;
}


};



