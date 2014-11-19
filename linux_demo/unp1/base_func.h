#define MAXN 16384

int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
	int 	listenfd, n;
	const int on = 1;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( (n = getaddrinfo(host, serv, &hints,&res)) != 0)
		fprintf(stderr,"tcp_listen error for %s, %s: %s", host, serv, gai_strerror(n));
	ressave = res;

	do {
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (listenfd < 0)
			continue;
		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		close(listenfd);
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)
		fprintf(stderr, "tcp_listen error for %s, %s", host, serv);

	listen(listenfd, LISTENQ);

	if (addrlenp)
		*addrlenp = res->ai_addrlen;

	freeaddrinfo(ressave);

	return listenfd;	
}

/*  host: services name or ip address , serv : services name or port */
int tcp_connect(const char *host, const char *serv)
{
	int	sockfd, n;
	struct 	addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( ( n = getaddrinfo(host, serv, &hints, &res)) != 0 )
		fprintf(stderr,"=== tcp_connect error for %s, %s: %s", host, serv, gai_strerror(n));
	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0 )
			continue;

		if (connect( sockfd, res->ai_addr, res->ai_addrlen) == 0 )
			break;
			
		close(sockfd);
	} while ( ( res = res->ai_next) != NULL);

	if ( res == NULL)
		fprintf(stderr, "+++++tcp_connect error for %s, %s\n",host, serv),exit(1);
	fprintf(stderr, " tcp_connect success\n");

	freeaddrinfo(ressave);

	return sockfd;
}

ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t nread;
	char 	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0){
		if ( (nread = read(fd, ptr, nleft)) < 0){
			if (errno == EINTR)
				nread = 0;
			else
				return -1;
		} else if (nread == 0 )
			break;
		nleft -= nread;
		ptr += nread;
	}
	return n - nleft;	
}


ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t n, rc;
	char 	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
	again:
		if ( (rc = read(fd, &c, 1) ) == 1){
			*ptr++ = c;
			if (c == '\n')
				break;
		}else if (rc == 0){
			*ptr = 0;
			return (n-1);
		} else {
			if ( errno == EINTR)
				goto again;
			return -1;
		}
	}
	*ptr = 0;
	return n;
}

void web_child(int sockfd)
{
	int 	ntowrite;
	ssize_t nread;
	char	line[MAXLINE], result[MAXLINE];
	
//	fprintf(stderr, "web_child ====1===\n");
printf("web_child ====1===\n");
	for ( ; ; ) {
		if ( (nread = readline(sockfd, line, MAXLINE)) == 0 )
			return; 

		ntowrite = atol(line);
//		fprintf(stderr,"receive :%s\n",line);
printf("receive :%s\n",line);
		if ( (ntowrite <= 0) || (ntowrite > MAXN))
			fprintf(stderr, "client request for %d bytes", ntowrite),exit(1);

		write(sockfd, result,ntowrite);
//		fprintf(stderr, "web_child ====2===\n");
printf("web_child ====2===\n");
	}
//	fprintf(stderr, "web_child ====3===\n");
printf( "web_child ====3===\n");
}


void pr_cpu_time(void)
{
	double	user, sys;
	struct rusage 	myusage, childusage;
	
	if (getrusage(RUSAGE_SELF, &myusage) < 0 )
		fprintf(stderr, "getrusage error"), exit(1);
	if (getrusage(RUSAGE_CHILDREN, &childusage) < 0 )
		fprintf(stderr, "getrusage error"), exit(1);

	user = (double) myusage.ru_utime.tv_sec + myusage.ru_utime.tv_usec/ 1000000.0;
	user += (double) childusage.ru_utime.tv_sec + childusage.ru_utime.tv_usec/ 1000000.0;
	sys = (double) myusage.ru_stime.tv_sec + myusage.ru_stime.tv_usec/ 1000000.0;
	sys += (double) childusage.ru_stime.tv_sec + childusage.ru_stime.tv_usec/ 1000000.0;

	printf("\nuser time = %g, sys time = %g\n", user,sys);
}

ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
	struct msghdr msg;
	struct iovec iov[1];
	ssize_t n;

#ifdef HAVE_MSGHDR_MSG_CONTROL
	union {
		struct cmsghdr cm;
		char	control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
#else
	int	newfd;
	
	msg.msg_accrights = (caddr_t) &newfd;
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	if ( (n = recvmsg(fd, &msg, 0)) < 0)
		return n;
	
#ifdef HAVE_MSGHDR_MSG_CONTROL
	if ( (cmptr = CMSG_FIRSTHDR(&msg)) != NULL && cmptr->cmsg_len == CMSG_LEN(sizeof(int))) {
		if (cmptr->cmsg_level != SOL_SOCKET)
			fprintf(stderr,"control level != SOL_SOCKET"), exit(1);
		if (cmptr->cmsg_type != SCM_RIGHTS)
			fprintf(stderr, "control type != SCM_RIGHTS"), exit(1);
		*recvfd = *((int *) CMSG_DATA(cmptr));
	} else
		*recvfd = -1;
#else
	if (msg.msg_accrightslen == sizeof(int))
		*recvfd = newfd;
	else
		*recvfd = -1;
#endif
	return n;
}	
	

ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
	struct msghdr msg;
	struct iovec iov[1];
#ifdef	HAVE_MSGHDR_MSG_CONTROL
	union {
		struct cmsghdr cm;
		char	control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr *cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
	
	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level =SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*((int *) CMSG_DATA(cmptr)) = sendfd;
#else
	msg.msg_accrights = (caddr_t) &sendfd;
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	
	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen =1;

	return sendmsg(fd, &msg, 0);
}


		
