
#include <sys/epoll.h>


#define MIO_FUNCS \
	static int _mio_poll(mio_t m, int t)								\
	{																	\
		return epoll_wait(MIO(m)->epoll_fd,								\
						  MIO(m)->res_event, 32, t*1000);				\					  
	}																	\
																		\
	static mio_fd_t _mio_alloc_fd(mio_t m, int fd)						\
	{																	\
		struct epoll_event event;										\
		mio_priv_fd_t priv_fd = malloc(sizeof(struct mio_priv_fd_st));  \
		memset(priv_fd, 0, sizeof(struct mio_priv_fd_st))				\
																		\
		priv_fd->mio_fd.fd = fd;										\
		priv_fd->events = 0;											\
																		\
		event.events = priv_fd->events;									\
		event.data.u64 = 0;												\
		event.data.ptr = priv_fd;										\
		epoll_ctl(MIO(m)->epoll_fd, EPOLL_CTL_ADD, fd, &event);			\
																		\
		return (mio_fd_t)priv_fd;										\
	}

#define MIO_FD_VARS \
	uint32_t events;

#define MIO_VARS \
	int defer_free;														\
	int epoll_fd;														\
	struct epoll_event res_event[32];

#define MIO_INIT_VARS(m) \
	do {																\
		MIO(m)->defer_free = 0;											\
		if ((MIO(m)->epoll_fd = epoll_create(maxfd)) < 0)				\
		{																\
			fprintf(stderr, "unable to initialize epoll mio");			\
			free(m);													\
			return NULL;												\
		}																\
	}while(0)

#define MIO_FREE_VARS(m) \
	do {																\
		close(MIO(m)->epoll_fd);										\
	}while(0)

#define MIO_ALLOC_FD(m, rfd) 	_mio_alloc_fd(m,rfd)
#define MIO_FREE_FD(m, mfd) 	if(mfd) free(mfd)

#define MIO_REMOVE_FD(m, mfd) \
	do {																\
		struct epoll_event event;										\
		event.events = 0;												\
		event.data.u64 = 0 ;											\
		event.data.ptr = mfd;											\
		epoll_ctl(MIO(m)->epoll_fd, EPOLL_CTL_DEL,						\
					mfd->mio_fd.fd, &event);							\
	}while(0)

#define MIO_SET_READ(m, mfd) \
	do {																\
		struct epoll_event event;										\
		mfd->events |= EPOLLIN;											\		
		event.events = mfd->events;										\
		event.data.u64 = 0;												\
		event.data.ptr = mfd;											\
		epoll_ctl(MIO(m)->epoll_fd, EPOLL_CTL_MOD, 						\
					mfd->mio_fd.fd, &event);							\
	}while(0)
		
#define MIO_SET_WRITE(m,mfd) \
	do {																\
		struct epoll_event event;										\
		mfd->events |=EPOLLOUT;											\
		event.events = mfd->events;										\
		event.data.u64 = 0;												\
		event.data.ptr = mfd;											\
		epoll_ctl(MIO(m)->epoll_fd, EPOLL_CTL_MOD,						\
					mfd->mio_fd.fd, &event);							\
	}while(0)					

#define MIO_UNSET_READ(m, mfd) \
	do {																\
		struct epoll_event event;										\
		mfd->events &= ~EPOLLIN;										\
		event.events = mfd->events;										\
		event.data.u64 = 0;												\
		event.data.ptr = mfd;											\
		epoll_ctl(MIO(m)->epoll_fd, EPOLL_CTL_MOD,						\
					mfd->mio_fd.fd, &event);							\
	}while(0)

#define MIO_UNSET_WRITE(m, mfd) \
	do {																\
		struct epoll_event event;										\
		mfd->events &= ~(EPOLLOUT);										\
		event.events = mfd->events;										\
		event.data.u64 = 0;												\
		event.data.ptr = mfd;											\
		epoll_ctl(MIO(m)->epoll_fd, EPOLL_CTL_MOD, 						\
					mfd->mio_fd.fd, &event);							\
	}while(0)
	
#define MIO_CAN_READ(m,iter) \
	(MIO(m)->res_event[iter].events & (EPOLLIN | EPOLLERR | EPOLLHUP))

#define MIO_CAN_WRITE(m, iter) \
	(MIO(m)->res_event[iter].events & EPOLLOUT)

#define MIO_CAN_FREE(m)  (!MIO(m)->defer_free)

#define MIO_INIT_ITERATOR(iter) \
	int iter

#define MIO_ITERATE_RESULTS(m, retval, iter) \
	for(MIO(m)->defer_free = 1, iter = 0; (iter < retval) || ((MIO(m)->defer_free = 0)); iter++)

#define MIO_ITERATOR_FD(m, iter) \
	(MIO(m)->res_event[iter].data.ptr)

