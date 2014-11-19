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
#include "skel.h"
#include "heartbeat.h"

char *program_name; 



void error( int status, int err, char *fmt, ... )
{
	va_list ap;
	
	va_start( ap, fmt );
	fprintf( stderr, "%s: ", program_name );
	vfprintf( stderr, fmt, ap );
	va_end( ap );
	if( err )
		fprintf( stderr, "%s (%d)\n", strerror( err ), err);
	if( status )
		EXIT( status );
}


static void set_address(char *hname, char *sname, struct sockaddr_in *sap, char *protocol)
{
	struct servent *sp;
	struct hostent *hp;
	char *endptr;
	short port;

	bzero( sap, sizeof( *sap ) );
	sap->sin_family = AF_INET ;
	if( hname != NULL )
	{
		if( !inet_aton( hname, &sap->sin_addr ))
		{
			hp = gethostbyname ( hname );
			if( hp == NULL )
				error( 1, 0, "unkonwn host: %s\n", hname);
			sap->sin_addr = * (struct in_addr *)hp->h_addr;
		}
	}
	else
		sap->sin_addr.s_addr = htonl( INADDR_ANY );
	port = strtol( sname, &endptr, 0 );
	if( *endptr == '\0' )
		sap->sin_port = htons( port );
	else
	{
		sp = getservbyname( sname, protocol );
		if( sp == NULL )
			error( 1, 0 ,"unkown service :%s\n", sname );
		sap->sin_port = sp->s_port ;
	}
}



/* serve - place holder for server */
#if 0
static void client(SOCKET s, struct sockaddr_in *peerp)
{

//	send(s, "hello, world\n",13,0);
}
#endif 

SOCKET tcp_client( char *hname, char *sname)
{
	struct sockaddr_in  peer;
	SOCKET s;

	set_address( hname, sname, &peer, "tcp");
	s = socket( AF_INET,SOCK_STREAM, 0 );
	if( !isvalidsock( s))
		error( 1, errno, "isvalidsock failed");
	if( connect( s, ( struct sockaddr *)&peer, sizeof(peer)))
		error( 1, errno, "connect failed");

	return s;
}


int main(int argc, int **argv)
{
	fd_set allfd;
	fd_set readfd;
	msg_t msg;
	struct timeval tv;
	SOCKET 	s;
	int rc;
	int heartbeats = 0 ;
	int cnt = sizeof(msg);

	INIT();
	s = tcp_client( argv[1], argv[2]);
	FD_ZERO( &allfd );
	FD_SET( s, &allfd );
	tv.tv_sec = T1 ;
	tv.tv_usec = 0 ;
	for( ; ; )
	{
		readfd = allfd ;
		rc = select( s+1 , &readfd, NULL , NULL, &tv );
		if(rc < 0 )
			error(1, errno, "select failure");
		if( rc == 0 )
		{
			if( ++heartbeats > 3 )
				error(1, 0, " connection dead \n");
			error( 0, 0, " sending heartbeat #%d\n",heartbeats );
			msg.type = htonl( MSG_HEARTBEAT );
			rc = send( s, (char *)&msg, sizeof( msg) ,0 );
			if( rc < 0)
				error(1, errno ," send failure");
			tv.tv_sec = T2 ;
			continue ;
		}
		
		if( ! FD_ISSET( s ,&readfd ))
			error( 1, 0 ,"select returned invalid socket\n");
		rc = recv( s, (char *)&msg + sizeof( msg ) - cnt , cnt , 0 );
		if( rc == 0 )
			error( 1, 0 , "server terminated\n");
		if( rc < 0 )
			error( 1, errno , "recv failure \n");
		heartbeats = 0 ;
		tv.tv_sec = T1 ;
		cnt -= rc ;
		if( cnt > 0 )
			continue ;
		cnt = sizeof( msg );
	}
}

	
			






