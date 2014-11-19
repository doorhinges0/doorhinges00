


/** our internal wrapper around a fd */
typedef enum { 
    type_CLOSED = 0x00, 
    type_NORMAL = 0x01, 
    type_LISTEN = 0x02, 
    type_CONNECT = 0x10, 
    type_CONNECT_READ = 0x11,
    type_CONNECT_WRITE = 0x12
} mio_type_t;
	
typedef struct mio_priv_fd_st
{
    struct mio_fd_st mio_fd;

    mio_type_t type;
    /* app event handler and data */
    mio_handler_t app;
    void *arg;

    MIO_FD_VARS
} *mio_priv_fd_t;

/** now define our master data type */
typedef struct mio_priv_st
{
    struct mio_st *mio;

    int maxfd;
    MIO_VARS
} *mio_priv_t;


/* lazy factor */
#define MIO(m) ((mio_priv_t) m)
#define FD(m,f) ((mio_priv_fd_t) f)
#define ACT(m,f,a,d) (*(FD(m,f)->app))(m,a,&FD(m,f)->mio_fd,d,FD(m,f)->arg)


/**add and set up this fd to this mio */
static mio_fd_t _mio_setup_fd(mio_t m, int fd, mio_handler_t app, void *arg)
{
	int flags;
	mio_fd_t mio_fd;

	fprintf(stderr, "adding fd #%d", fd);

	mio_fd = MIO_ALLOC_FD(m, fd);
	if (mio_fd == NULL) return NULL;

	/* ok to process this one, welcome to the family*/
	FD(m,mio_fd)->type = type_NORMAL;
	FD(m,mio_fd)->app = app;
	FD(m,mio_fd)->arg = arg;

	/*set the socket to non-blocking */
	flags = fcntl(fd, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(fd, F_SETFL, flags);

	return mio_fd;


}

/** internal  close function */
static void _mio_close(mio_t m, mio_fd_t fd)
{
	if(FD(m,fd)->type == type_CLOSED)
		return ;

	fprintf(stderr, "actually closing fd #%d", fd->fd);

	/* take out of poll sets */
	MIO_REMOVE_FD(m, FD(m,fd));

	/* let the app know, it must process any waiting write data it has and free it's arg*/
	if (FD(m,fd)->app != NULL)
		ACT(m, fd, action_CLOSE, NULL);

	/* close the socket, and reset all memory */
	close(fd->fd);
	FD(m,fd)->type = type_CLOSED;
	FD(m,fd)->app = NULL;
	FD(m,fd)->arg = NULL;

	if (MIO_CAN_FREE(m))
	{
		MIO_FREE_FD(m, fd);
	}
}


/** internally accept an incoming connection from a listen sock*/
static void _mio_accept(mio_t m, mio_fd_t fd)
{
	struct sockadd_storage serv_addr;
	socklen_t addrlen = (socklen_t) sizeof(serv_addr);
	int newfd;
	mio_fd_t mio_fd;
	char ip[INET6_ADDRSTRLEN];

	fprintf(stderr, "accepting on fd #%d", fd->fd);

	/* pull a socket off the accept queue and check*/
	newfd = accept(fd->fd, (struct sockaddr*)&serv_addr, &addrlen);
	if(newfd <= 0) return ;
	if(addrlen <= 0) {
		close(newfd);
		return ;
	}
	
	inet_ntop(&serv_addr, ip, sizeof(ip));
	fprintf(stderr, "new socket accepted fd #%d, %s:%d", newfd, ip, inet_getport(&serv_addr);

	/* set up the entry for this new  socket */
	mio_fd = _mio_setup_fd(m, newfd, FD(m,fd)->app, FD(m,fd)->arg);

	if(!mio_fd) {
		close(newfd);
		return;
	}

	/* tell the app about the new socket, if they reject it clean up */

}


