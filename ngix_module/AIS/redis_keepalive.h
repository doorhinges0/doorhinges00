#ifndef __REDIS_KEEPALIVE_H_20140522__
#define __REDIS_KEEPALIVE_H_20140522__

#include "redisc/hiredis.h"

namespace rediskeepalive{
void send_redis_command(redisContext *c, const char *format, ...);
redisContext *__AsyncConnect(redisContext *c, const char *ip, int port);
int __AsyncHandleConnect(redisContext *c, int &err);
int __CheckSocketError(redisContext *c, int fd, int &err);
int __onConnect(redisContext *c);
void __redisContextInit(redisContext *c);
int __disConnect(redisContext *c);
void __releaseResource(redisContext *c);
void __SetErrorFromErrno(redisContext *c, int type, const char *prefix);
void __SetError(redisContext *c, int type, const char *str);  
int __ContextConnectTcp(redisContext *c, const char *addr, int port, struct timeval *timeout);
int __SetBlocking(redisContext *c, int fd, int blocking);
int __SetTcpNoDelay(redisContext *c, int fd);

};
#endif//__REDIS_KEEPALIVE_H_20140522__
