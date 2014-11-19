#include "mio.h"

mio_t mio_new(int maxfd)
{
  mio_t m = NULL;

  
#ifdef MIO_EPOLL
	m = mio_epoll_new(maxfd);
	if (m != NULL) return m;
#endif

	return m;
}






