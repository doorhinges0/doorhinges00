
#ifndef _NETWORK_SOCK_H_
#define _NETWORK_SOCK_H_

#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <string>
#include <stdexcept>

//////////////////////////////////////////////////////////////////////////

#define THROW_SOEXP(exp, err_str) ( throw exp( __FILE__ , __LINE__ , errno , err_str ) )
#define THROW_SOEXP_SIMP(function) (THROW_SOEXP( socket_error , " " + std::string( #function ) + " " + strerror(errno)))

namespace network
{
	static inline std::string i2s(int i){char s[12];sprintf(s,"%d",i);return s;}
	
	class socket_error: public std::runtime_error
	{
	public:
		socket_error(const char* file, int line, const std::string& err_str)
			:std::runtime_error(std::string(file) + i2s(line) + err_str), _err_no(0){}
		socket_error(const char* file, int line, int err_no, const std::string& err_str)
			:std::runtime_error(std::string(file) + i2s(line) + err_str), _err_no(_err_no){}
		const int _err_no;
	};
	
	class socket_nil: public socket_error
	{
	public:
		socket_nil(const char* file, int line, int err_no, const std::string& err_str)
			:socket_error(file, line, err_no, err_str){}
	};
	
	typedef in_addr_t numeric_ipv4_t;
	typedef sa_family_t family_t;
	typedef uint16_t port_t;
	
	class CSocketAddr
	{
	public:
		CSocketAddr():_len(sizeof(struct sockaddr_in))
		{memset(&_addr, 0, sizeof(struct sockaddr_in));}
		
		struct sockaddr * addr(){return (struct sockaddr *)(&_addr);}
		struct sockaddr_in * addr_in(){return &_addr;}
		socklen_t& length(){return _len;}
		
		numeric_ipv4_t get_numeric_ipv4(){return _addr.sin_addr.s_addr;}
		void set_numeric_ipv4(numeric_ipv4_t ip){_addr.sin_addr.s_addr = ip;}
		
		port_t get_port(){return ntohs(_addr.sin_port);}
		void set_port(port_t port){_addr.sin_port = htons(port);}
		
		family_t get_family(){return _addr.sin_family;}
		void set_family(family_t f){_addr.sin_family = f;}
		
		static std::string in_n2s(numeric_ipv4_t addr);
		static numeric_ipv4_t in_s2n(const std::string& addr);
		
		private:
			struct sockaddr_in _addr;
			socklen_t _len;
	};
	
	class CSocket
	{
		// Construction
	public:
		CSocket():_socket_fd(INVALID_SOCKET){}
		~CSocket(){close();}
		void create(int protocol_family = PF_INET, int socket_type = SOCK_STREAM);
		
		// Attributes, accessor & mutator
	public:
		int fd() const {return _socket_fd;};
		bool socket_is_ok() const {return (_socket_fd != INVALID_SOCKET);}
		bool test_read();
		
		void get_peer_name(std::string& peer_address, port_t& peer_port);
		void get_peer_name(numeric_ipv4_t& peer_address, port_t& peer_port);
		void get_sock_name(std::string& socket_address, port_t& socket_port);
		void get_sock_name(numeric_ipv4_t& socket_address, port_t& socket_port);
		
		// Operations
	public:
		void bind(port_t port, const std::string& server_address = "");
		void listen(int backlog = 32);
		void connect(const std::string & addr, port_t port , int wait = 0 );
		void udp_connect(const std::string & addr, port_t port );
		void accept(CSocket& client_socket);
		void close();

		int block_recv( void *buf , size_t len , int wait , int flag /* = 0 */);
		int block_recv_n( void *buf , size_t len , int wait , int flag /* = 0 */);

		int receive	(void * buf, size_t len, int flag = 0);
		int receive_from	(void * buf, size_t len,
			std::string& from_address, port_t& from_port, int flags = 0);
		int receive_from	(void * buf, size_t len,
			numeric_ipv4_t& from_address, port_t& from_port, int flags = 0);
		
		int send	(const void * buf, size_t len, int flag = 0);
		int send_to			(const void *msg, size_t len,
			const std::string& to_address, port_t to_port, int flags = 0);
		int send_to			(const void *msg, size_t len,
			numeric_ipv4_t to_address, port_t to_port, int flags = 0);
		
		void shutdown(int how = SHUT_WR);
		void set_nonblock();
		void set_reuseaddr();
		void ignore_pipe_signal();
		
	protected:
		static const int INVALID_SOCKET = -1;
		int _socket_fd;
		
	private:
		CSocket(const CSocket& sock); // no implementation
		void operator = (const CSocket& sock); // no implementation
	};
	
	//////////////////////////////////////////////////////////////////////////
	
	inline std::string CSocketAddr::in_n2s(numeric_ipv4_t addr)
	{
		char buf[INET_ADDRSTRLEN];
		const char* p = inet_ntop(AF_INET, &addr, buf, sizeof(buf));
		return p ? p : std::string();
	}
	
	inline numeric_ipv4_t CSocketAddr::in_s2n(const std::string& addr)
	{
		struct in_addr sinaddr;
		errno = 0;
		int ret = inet_pton(AF_INET, addr.c_str(), &sinaddr);
		
		if (ret < 0)
			THROW_SOEXP(socket_error, std::string("CSocketAddr::in_s2n: inet_pton: ")
			+ addr + strerror(errno));
		else if (ret == 0)
			throw socket_error(__FILE__, __LINE__, std::string(
			"CSocketAddr::in_s2n: inet_pton: does not contain a character "
			"string representing a valid network address in: ") + addr);
		
		return sinaddr.s_addr;
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	inline void CSocket::create(int protocol_family /* = PF_INET */, int socket_type /* = SOCK_STREAM */)
	{
		//	release fd first
		close();
		
		//	create and check
		_socket_fd =::socket(protocol_family, socket_type, 0);
		if (_socket_fd < 0)
		{
			THROW_SOEXP(socket_error, std::string("CSocket::create: ") + strerror(errno));
			_socket_fd = INVALID_SOCKET;
		}
	}
	
	inline void CSocket::listen(int backlog /*=32*/)
	{
		if (::listen(_socket_fd, backlog) < 0)
			THROW_SOEXP(socket_error, std::string("CSocket::listen ")+strerror(errno));
	}
	
	inline void CSocket::accept(CSocket & client_socket)
	{
		client_socket.close();
		int client_fd = INVALID_SOCKET;
		errno = 0;
		client_fd =::accept(_socket_fd, NULL, NULL);
		if (client_fd < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				THROW_SOEXP(socket_nil, std::string(" CSocket::accept: ") + strerror(errno));
			else
				THROW_SOEXP_SIMP(CSocket::accept);
		}
		client_socket._socket_fd = client_fd;
	}
	
	inline void CSocket::bind(port_t port, const std::string &server_address /* = "" */ )
	{
		CSocketAddr addr;
		addr.set_family(AF_INET);
		addr.set_port(port);
		
		numeric_ipv4_t ip = server_address.empty() ? htonl(INADDR_ANY)
			: CSocketAddr::in_s2n(server_address);
		addr.set_numeric_ipv4(ip);
		
		errno = 0;
		if (::bind(_socket_fd, addr.addr(), addr.length()) < 0)
			THROW_SOEXP_SIMP(CSocket::bind);
	}
	
	inline void CSocket::connect(const std::string& address, port_t port , int wait )
	{
		CSocketAddr addr;
		addr.set_family(AF_INET);
		addr.set_port(port);
		addr.set_numeric_ipv4(CSocketAddr::in_s2n(address));
		errno = 0;

		if( wait == 0 )
		{
			if (::connect(_socket_fd, addr.addr(), addr.length()) < 0)
				THROW_SOEXP_SIMP(CSocket::connect);
		}
		else
		{
			int val = fcntl(_socket_fd, F_GETFL, 0);

                	if (val == -1) THROW_SOEXP_SIMP(CSocket::set_connect);

			if ( ! val & O_NONBLOCK )
			{
               			 if (fcntl(_socket_fd, F_SETFL, val | O_NONBLOCK | O_NDELAY) == -1)
                     			   THROW_SOEXP_SIMP(CSocket::set_nonblock);
			}
				
			if (::connect(_socket_fd, addr.addr(), addr.length()) < 0 && errno != EINPROGRESS )
				THROW_SOEXP_SIMP(CSocket::connect);

			fd_set rfds;
			struct timeval tv;
			
			FD_ZERO(&rfds);
           		FD_SET(_socket_fd, &rfds);

		        tv.tv_sec = wait;
           		tv.tv_usec = 0;			

			int retval;
			retval = select( _socket_fd + 1 , NULL , &rfds, NULL, &tv);

		        if ( retval < 0 && errno != EINTR )
				THROW_SOEXP_SIMP(CSocket::connect);

			if (fcntl(_socket_fd, F_SETFL, val ) == -1)
                        	THROW_SOEXP_SIMP(CSocket::set_nonblock);
		}
	}

	inline void  CSocket::udp_connect(const std::string & address, port_t port )
	{
		CSocketAddr addr;
		addr.set_family(AF_INET);
		addr.set_port(port);
		addr.set_numeric_ipv4(CSocketAddr::in_s2n(address));
		errno = 0;
		
		if (::connect(_socket_fd, addr.addr(), addr.length()) < 0)
				THROW_SOEXP_SIMP(CSocket::connect);
		
	}

	inline int CSocket::block_recv( void *buf , size_t len , int wait , int flag = 0 )
	{
		errno = 0;
		size_t recvlen = 0;
		

		fd_set rfds;
		struct timeval tv;
		int retval;
	
		FD_ZERO( &rfds );
		FD_SET( _socket_fd , &rfds );

		tv.tv_sec = wait;
		tv.tv_usec = 0;

		time_t t1 = time( NULL );
		retval = select( _socket_fd + 1 , &rfds , NULL , NULL , &tv );
		time_t t2 = time( NULL );
			
		wait -= t2 - t1; 

		if( retval == -1 || retval == 0 )
			return -1;

		recvlen = receive( (char*)buf + recvlen , len - recvlen , flag );
		return recvlen;
	}
	
	inline int CSocket::block_recv_n( void *buf , size_t len , int wait , int flag = 0 )
	{
		errno = 0;
		size_t recvlen = 0;
		
		while( true )
		{
			fd_set rfds;
			struct timeval tv;
			int retval;
	
			FD_ZERO( &rfds );
			FD_SET( _socket_fd , &rfds );

			tv.tv_sec = wait;
			tv.tv_usec = 0;

			time_t t1 = time( NULL );
			retval = select( _socket_fd + 1 , &rfds , NULL , NULL , &tv );
			time_t t2 = time( NULL );
			
			wait -= t2 - t1; 

			if (retval == -1)
				return -1;
			else if (retval)
			{
				recvlen += receive( (char*)buf + recvlen , len - recvlen , flag );
				if( recvlen == len )
				{
					return recvlen;
				}
			}
		}

		return 0;
	}

	inline bool CSocket::test_read()
	{
		errno = 0;
		char szTmp[ 32 ] = { 0 };
		int bytes = ::recv( _socket_fd , szTmp , sizeof( szTmp ) , MSG_PEEK );
		if ( bytes == 0 )
			return false;

		return true; 
	}
	
	inline int CSocket::receive(void *buf, size_t len, int flag /* = 0 */)
	{
		errno = 0;
		int bytes =::recv(_socket_fd, buf, len, flag);
		if ( bytes < 0)	THROW_SOEXP_SIMP(CSocket::receive);
		return bytes;
	}
	
	inline int CSocket::receive_from(void * buf, size_t len, std::string& from_address,
						  port_t& from_port, int flags /* = 0 */)
	{
		numeric_ipv4_t ip = 0;
		int bytes = receive_from(buf, len, ip, from_port, flags);
		from_address = CSocketAddr::in_n2s(ip);
		return bytes;
	}
	
	inline int CSocket::receive_from(void * buf, size_t len, numeric_ipv4_t& from_address,
						  port_t& from_port, int flags /* = 0 */)
	{
		CSocketAddr addr;
		errno = 0;
		int bytes =::recvfrom(_socket_fd, buf, len, flags, addr.addr(), &addr.length());
		if (bytes < 0)	THROW_SOEXP_SIMP(CSocket::receive_from);
		from_address = addr.get_numeric_ipv4();
		from_port = addr.get_port();
		return bytes;
	}
	
	inline int CSocket::send(const void *buf, size_t len, int flag)
	{
		errno = 0;
		int bytes = ::send(_socket_fd, buf, len, flag);
		if (bytes < 0)	THROW_SOEXP_SIMP(CSocket::send);
		return bytes;
	}
	
	inline int CSocket::send_to(const void *msg, size_t len,
		const std::string& to_address, port_t to_port,
		int flags /* = 0 */)
	{
		return send_to(msg, len, CSocketAddr::in_s2n(to_address), to_port, flags);
	}
	
	inline int CSocket::send_to(const void *msg, size_t len,
		numeric_ipv4_t to_address, port_t to_port,
		int flags /* = 0 */)
	{
		CSocketAddr addr;
		addr.set_family(AF_INET);
		addr.set_numeric_ipv4(to_address);
		addr.set_port(to_port);
		errno = 0;
		int bytes =::sendto(_socket_fd, msg, len, flags, addr.addr(), addr.length());
		if (bytes < 0)	THROW_SOEXP_SIMP(CSocket::send_to);
		return bytes;
	}
	
	inline void CSocket::shutdown(int how/* = SHUT_WR*/)
	{
		if (::shutdown(_socket_fd, how) < 0)
			THROW_SOEXP(socket_error, std::string("CSocket::shutdown: ") + strerror(errno));
	}
	
	inline void CSocket::close()
	{
		if (_socket_fd != INVALID_SOCKET)
		{
			::close(_socket_fd);
			_socket_fd = INVALID_SOCKET;
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	inline void CSocket::get_peer_name(std::string & peer_address, port_t & peer_port)
	{
		CSocketAddr addr;
		if (::getpeername(_socket_fd, addr.addr(), &addr.length()) < 0)
			THROW_SOEXP_SIMP(CSocket::get_peer_name);
		peer_address = CSocketAddr::in_n2s(addr.get_numeric_ipv4());
		peer_port = addr.get_port();
	}
	
	inline void CSocket::get_peer_name(numeric_ipv4_t & peer_address, port_t & peer_port)
	{
		CSocketAddr addr;
		if (::getpeername(_socket_fd, addr.addr(), &addr.length()) < 0)
			THROW_SOEXP_SIMP(CSocket::get_peer_name);
		peer_address = addr.get_numeric_ipv4();
		peer_port = addr.get_port();
	}
	
	inline void CSocket::get_sock_name(std::string & socket_address, port_t & socket_port)
	{
		CSocketAddr addr;
		if (::getsockname(_socket_fd, addr.addr(), &addr.length()) < 0)
			THROW_SOEXP_SIMP(CSocket::get_sock_name);
		socket_address = CSocketAddr::in_n2s(addr.get_numeric_ipv4());
		socket_port = addr.get_port();
	}
	
	inline void CSocket::get_sock_name(numeric_ipv4_t& socket_address, port_t & socket_port)
	{
		CSocketAddr addr;
		if (::getsockname(_socket_fd, addr.addr(), &addr.length()) < 0)
			THROW_SOEXP_SIMP(CSocket::get_sock_name);
		socket_address = addr.get_numeric_ipv4();
		socket_port = addr.get_port();
	}
	
	inline void CSocket::set_reuseaddr()
	{
		int optval = 1;
		size_t optlen = sizeof(optval);
		if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen) < 0)
			THROW_SOEXP_SIMP(CSocket::set_reuseaddr);
	}

	inline void CSocket::set_nonblock()
	{
		int val = fcntl(_socket_fd, F_GETFL, 0);
		
		if (val == -1) THROW_SOEXP_SIMP(CSocket::set_nonblock);
		
		if (val & O_NONBLOCK)	
			return;
		
		if (fcntl(_socket_fd, F_SETFL, val | O_NONBLOCK | O_NDELAY) == -1)
			THROW_SOEXP_SIMP(CSocket::set_nonblock);
	}
	
	inline void CSocket::ignore_pipe_signal()
	{
		signal(SIGPIPE, SIG_IGN);
	}
};

class CSockAttacher : public network::CSocket
{
public:
	CSockAttacher(int fd){_socket_fd = fd;}
	~CSockAttacher(){detach_fd();}
	
	void attach_fd(int fd){_socket_fd = fd;}
	int detach_fd(){int fd = _socket_fd; _socket_fd = INVALID_SOCKET; return fd;}
};

inline unsigned short checksum( unsigned short* buffer, int size )
{ 
	uint32_t cksum = 0;

	while( size > 1 )
	{
		cksum+=*buffer++;
		size -=sizeof( unsigned short );
	}

	if( size ) 
		cksum += *(unsigned char*)buffer;

    cksum = (cksum >> 16) + (cksum & 0xffff);

	cksum += (cksum >>16);

	return ( unsigned short )( ~cksum );
}

inline unsigned short checksum( char* pBuffer , int nBufLen )
{
	//ASSERT(pBuffer && nBufLen > 0);
	
	uint32_t nSum = 0;
	unsigned short* pData = (unsigned short*)pBuffer;
	int nLen = nBufLen / 2;
	int nMod = nBufLen % 2;
	for(int i = 0;i < nLen;i++)
		nSum += pData[i];
	unsigned short nShort = 0;
	if(nMod == 1)
	{
		nShort = (unsigned short)pBuffer[nBufLen - 1];
		nSum += nShort;
	}
	nSum = (nSum >> 16) + (nSum & 0xffff);
	nSum += (nSum >> 16);
	nShort = ~((unsigned short)nSum);
	return nShort;
}

inline char *GetDateTimeStr(time_t *mytime)
{
    static char s[50];
    struct tm curr;
    
    curr = *localtime(mytime);

    if (curr.tm_year > 50)
    {
        sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d", 
                    curr.tm_year+1900, curr.tm_mon+1, curr.tm_mday,
                    curr.tm_hour, curr.tm_min, curr.tm_sec);
    }
    else    
    {
        sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d", 
                    curr.tm_year+2000, curr.tm_mon+1, curr.tm_mday,
                    curr.tm_hour, curr.tm_min, curr.tm_sec);
    }
        
    return s;
}

inline char *GetCurDateTimeStr(void)
{
    time_t  iCurTime;

    time(&iCurTime);
    return GetDateTimeStr(&iCurTime);
}


//////////////////////////////////////////////////////////////////////////
#endif//_NETWORK_SOCK_H_
///:~
