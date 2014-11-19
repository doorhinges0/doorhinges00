
#define _SS_PADSIZE (128-sizeof(sa_family_t))


#define sockaddr_storage {
	sa_family_t		ss_family;		/**< address family */
	char __ss_pad[_SS_PADSIZE];		/**< padding to a size of 128 bytes */
}

#ifndef INET6_ADDRSTRLEN
# define INET6_ADDRSTRLEN 46	/**< maximum length of the string
				  representation of an IPv6 address */
#endif



