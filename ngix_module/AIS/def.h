/************************************************************
 * 默认头文件
 *
 * Author: allen
 ***********************************************************/
#ifndef __BS_DEF_H
#define __BS_DEF_H
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <sys/un.h>

typedef int8_t		int8;
typedef int16_t		int16;
typedef int32_t		int32;
typedef int64_t		int64;
typedef u_int8_t	uint8;
typedef u_int16_t	uint16;
typedef u_int32_t	uint32;
typedef u_int64_t	uint64;


#undef assert
#define assert(BS_expr) { \
	if (!(BS_expr)) \
	{ \
		xLog(XLOG_FATAL, "assertion failed: %s", #BS_expr); \
		abort(); \
    } \
}

#define DEF_KEY_LEN 32

#endif
