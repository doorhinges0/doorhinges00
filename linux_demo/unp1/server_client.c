#include "common.h"
#include "base_func.h"

#define MAXN 	16384

int main(int argc, char **argv)
{
	int i, j, fd, nchildren, nloops, nbytes;
	pid_t pid;
	ssize_t n;
	char request[MAXLINE], reply[MAXN];
	
	if ( argc != 6)
		fprintf(stderr, "usage: server_client <hostname or ipaddr> <port> <#children> "
				" <#loops/child> <#bytes/request>");

	nchildren = atoi(argv[3]);
	nloops = atoi(argv[4]);
	nbytes = atoi(argv[5]);
	snprintf(request, sizeof(request),"%d\n",nbytes);

	for (i = 0 ; i < nchildren; i++) {
		if ( (pid = fork()) == 0 ) {
			for (j = 0 ; j < nloops; j++) {
				fd = tcp_connect(argv[1], argv[2]);

				write(fd, request, strlen(request));

				if ( (n = readn( fd, reply, nbytes)) != nbytes)
					fprintf(stderr, "server returned %d bytes",n),exit(1);

				close(fd);
			}
			printf("child %d done\n",i);
			exit(0);
		}
	}

	while (wait(NULL) > 0)
		;
	if (errno != ECHILD)
		fprintf(stderr, "wait error");

	exit(0);
}




