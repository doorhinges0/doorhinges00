

/**
 * get the string representation of an address in struct sockaddr_storage
 * (modeled after the stdlib function inet_ntop)
 *
 * @param src the struct sockaddr_storage where the address should be read
 * @param dst where to write the result
 * @param size the size of the result buffer
 * @return NULL if failed, pointer to the result otherwise
 */
const char *inet_ntop(struct sockaddr_storage *src, char *dst, size_t size)
{
#ifndef HAVE_INET_NTOP
    char *tmp;
    struct sockaddr_in *sin;

    sin = (struct sockaddr_in *)src;

    /* if we don't have inet_ntop we only accept AF_INET
     * it's unlikely that we would have use for AF_INET6
     */
    if(src->ss_family != AF_INET)
    {
        return NULL;
    }

    tmp = inet_ntoa(sin->sin_addr);

    if(!tmp || strlen(tmp)>=size)
    {
	return NULL;
    }

    strncpy(dst, tmp, size);
    return dst;
#else
    struct sockaddr_in *sin;
    struct sockaddr_in6 *sin6;

    sin = (struct sockaddr_in *)src;
    sin6 = (struct sockaddr_in6 *)src;

    switch(src->ss_family)
    {
    case AF_UNSPEC:
    case AF_INET:
        return inet_ntop(AF_INET, &sin->sin_addr, dst, size);
    case AF_INET6:
        return inet_ntop(AF_INET6, &sin6->sin6_addr, dst, size);
    default:
        return NULL;
    }
#endif
}

/**
 * get the port number out of a struct sockaddr_storage
 *
 * @param sa the struct sockaddr_storage where we want to read the port
 * @return the port number (already converted to host byte order!)
 */
int inet_getport(struct sockaddr_storage *sa)
{
    struct sockaddr_in *sin;
    struct sockaddr_in6 *sin6;
    
    switch(sa->ss_family)
    {
    case AF_INET:
        sin = (struct sockaddr_in *)sa;
        return ntohs(sin->sin_port);
    case AF_INET6:
        sin6 = (struct sockaddr_in6 *)sa;
        return ntohs(sin6->sin6_port);
    default:
        return 0;
    }
}



