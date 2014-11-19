#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define max(a, b) ((a)>(b)?(a):(b))
#define SEM_NAME "mysem"
#define SVSEM_MODE (SEM_R | SEM_A | SEM_R>>3 | SEM_R>>6)
#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP| S_IROTH )
#define NBUFF  10
#define SEM_MUTEX  "mutex"
#define SEM_NEMPTY "nempty"
#define SEM_NSTORED "nstored"
#define POSIX_IPC_PREFIX "/"
#ifndef PATH_MAX
#define PATH_MAX  1024
#endif