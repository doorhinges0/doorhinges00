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

#define MAXLINE 4096 


int main(int argc , char **argv)
{
	int fd[2], n;
	char c;
	pid_t childpid;

	pipe(fd);
	if( (childpid =fork()) == 0 ){ /*child*/
		sleep(3);
		if( (n = read(fd[0], &c, 1) )!= -1 )
			fprintf(stderr, "read error");
		printf(" child read %c\n",c);
		write(fd[0],"c",1);
		exit(0);
	}

	/*parent*/
	write(fd[1],"p" , 1 );
	if( ( n = read(fd[1],&c,1 ))!= 1)
		fprintf(stderr, "parent read error");
	printf(" parent read %c\n",c);
	exit(0);
}


	





