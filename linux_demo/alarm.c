#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void timeoutExit()
{
	printf("\n sorry , timeout and exit by 1!\n");
	exit(1);
}

int main()
{
	signal(SIGALRM,timeoutExit);
	alarm(30);
	
	int n ;
	scanf("%d",&n);
	printf("n=%d\n",n);
	printf("ok,the program si return by 0 !\n");
	return 0;
}


