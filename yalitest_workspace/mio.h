#include <stdio.h>   
#include <sys/types.h>   
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include<iostream>
#include<fcntl.h>


struct mio_st;



typedef struct mio_fd_st
{
    int fd;
} *mio_fd_t;

/** these are the actions and a handler type assigned by the applicaiton using mio */
typedef enum { action_ACCEPT, action_READ, action_WRITE, action_CLOSE } mio_action_t;
typedef int (*mio_handler_t) (struct mio_st **m, mio_action_t a, struct mio_fd_st *fd, void* data, void *arg);


typedef struct mio_st
{
  void (*mio_free)(struct mio_st **m);

  struct mio_fd_st *(*mio_listen)(struct mio_st **m, int port, const char *sourceip,
				  mio_handler_t app, void *arg);

  struct mio_fd_st *(*mio_connect)(struct mio_st **m, int port, const char *hostip,
				   const char *srcip, mio_handler_t app, void *arg);

  struct mio_fd_st *(*mio_register)(struct mio_st **m, int fd,
				   mio_handler_t app, void *arg);

  void (*mio_app)(struct mio_st **m, struct mio_fd_st *fd,
		  mio_handler_t app, void *arg);

  void (*mio_close)(struct mio_st **m, struct mio_fd_st *fd);

  void (*mio_write)(struct mio_st **m, struct mio_fd_st *fd);

  void (*mio_read)(struct mio_st **m, struct mio_fd_st *fd);

  void (*mio_run)(struct mio_st **m, int timeout);
} **mio_t;


 mio_t mio_new(int maxfd); /* returns NULL if failed */

 
#define mio_free(m) (*m)->mio_free(m)
 
 /** for creating a new listen socket in this mio (returns new fd or <0) */
#define mio_listen(m, port, sourceip, app, arg) \
	 (*m)->mio_listen(m, port, sourceip, app, arg)
 
 /** for creating a new socket connected to this ip:port (returns new fd or <0, use mio_read/write first) */
#define mio_connect(m, port, hostip, srcip, app, arg) \
	 (*m)->mio_connect(m, port, hostip, srcip, app, arg)
 
 /** for adding an existing socket connected to this mio */
#define mio_register(m, fd, app, arg) \
	 (*m)->mio_register(m, fd, app, arg)
 
 /** re-set the app handler */
#define mio_app(m, fd, app, arg) (*m)->mio_app(m, fd, app, arg)
 
 /** request that mio close this fd */
#define mio_close(m, fd) (*m)->mio_close(m, fd)
 
 /** mio should try the write action on this fd now */
#define mio_write(m, fd) (*m)->mio_write(m, fd)
 
 /** process read events for this fd */
#define mio_read(m, fd) (*m)->mio_read(m, fd)
 
 /** give some cpu time to mio to check it's sockets, 0 is non-blocking */
#define mio_run(m, timeout) (*m)->mio_run(m, timeout)
 




