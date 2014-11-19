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


int readn( SOCKET fd, char *bp, size_t len)
{
	int cnt;
	int rc;
	
	cnt = len ;
	while( cnt > 0)
	{
		rc = recv( fd, bp, cnt, 0 );
		if( rc < 0 )
		{
			if( errno == EINTR )
				continue ;
			return -1 ;
		}
		if( rc == 0 )
			return len - cnt ;
		bp += rc;
		cnt -= rc;
	}
	return len ;
}


int readvrec( SOCKET fd, char *bp, size_t len )
{
	u_int32_t reclen;
	int rc ;
	
	rc = readn( fd, (char *)&reclen, sizeof( u_int32_t ) );
	if( rc != sizeof( u_int32_t ) )
		return rc < 0 ? -1 : 0;
	reclen = ntohl( reclen );
	if( reclen > len )
	{
		while( reclen > 0 )
		{
			rc = readn( fd, bp, len );
			if( rc != len )
				return rc < 0 ? -1 : 0;
			reclen -= len;
			if( reclen < len )
				len = reclen;
		}
		set_errno( EMSGSIZE );
		return -1;
	}

	rc = readn( fd, bp, reclen );
	if( rc != reclen )
		return rc < 0 ? -1 : 0 ;
	return rc;
}
		

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
static void server(SOCKET s, struct sockaddr_in *peerp)
{

//	send(s, "hello, world\n",13,0);
}
#endif 

SOCKET tcp_server( char *hname, char *sname)
{
	struct sockaddr_in  local;
	SOCKET s;
	const int on = 1;

	set_address( hname, sname, &local, "tcp");
	s = socket( AF_INET,SOCK_STREAM, 0 );
	if( setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)))
		error( 1, errno, "setsockopt failed");
	if( bind( s, ( struct sockaddr *)&local, sizeof(local)))
		error( 1, errno, "bind failed");
	if(listen( s,NLISTEN))
		error( 1, errno, "listen failed");
	return s;
}


int main(int argc , char **argv )
{
	fd_set allfd ;
	fd_set readfd ;
	msg_t msg;
	struct timeval tv;
	SOCKET s ;
	SOCKET 	s1;
	int rc ; 
	int missed_heartbeats = 0 ;
	int cnt = sizeof( msg );

	INIT();
	s = tcp_server(NULL , argv[ 1] );
	s1 = accept( s, NULL , NULL );
	if( !isvalidsock( s1 ))
		error( 1, errno ,"accept failure")  ;
	tv.tv_sec = T1 +T2 ;
	tv.tv_usec = 0 ;
	FD_ZERO( &allfd );
	FD_SET( s1, &allfd ) ;
	for( ; ; )
	{
		readfd = allfd ;
		rc = select( s1 +1 , &readfd, NULL, NULL ,&tv );
		if( rc < 0)
			error( 1, errno , "select failure");
		if ( rc == 0 )
		{
			if( ++missed_heartbeats > 3 )
				error( 1, 0 , " connection dead\n");
			error( 0 , 0 , "missed heartbeat #%d\n", missed_heartbeats );
			tv.tv_sec = T2 ;
			continue;
		}

		if( !FD_ISSET( s1 , &readfd ))
			error( 1, 0, "select returned invalid socket\n");
		rc = recv( s1, (char *)&msg + sizeof(msg) - cnt, cnt ,0 );
		if( rc == 0 )
			error( 1, 0, "client terminated\n");
		if( rc < 0 )
			error( 1, errno , "recv failure");
		missed_heartbeats = 0 ;
		tv.tv_sec = T1 + T2 ;
		cnt -= rc ;
		if( cnt > 0 )
			continue;
		cnt = sizeof( msg );
		switch( ntohl( msg.type))
		{
			case MSG_TYPE1 :
				break; 
			case MSG_TYPE2 :
				break ;
			case MSG_HEARTBEAT :
				rc = send( s1, (char *)&msg, sizeof( msg), 0 );
				if( rc < 0 )
					error( 1, errno, "send failure");
				break; 
			default :
				error( 1, 0, "unkonwn message type (%d) \n",ntohl( msg.type));
		}
	}

	EXIT( 0 );
}
	
			
		