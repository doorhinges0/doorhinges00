CC = gcc
CXX = g++
PROG	= ais
#CFLAGS =  -Wall  -std=gnu99  -DHAVE_CONFIG_H -DDBUG -DREDIS -DTXT_LOG    -I. -g   -O2
#CFLAGS =  -Wall  -std=gnu99  -DHAVE_CONFIG_H -DEBUG -DREDIS -DTXT_LOG -DTEST    -I. -g   -O2


CFLAGS =  -Wall  -std=gnu99  -DHAVE_CONFIG_H  -DREDIS -DTXT_LOG  -DTEST  -I/usr/local/include -I. -g -O3 
CFLAGS_CXX =  -Wall -Wno-sign-compare -Wno-unused -Wno-write-strings -Wno-pointer-arith -DHAVE_CONFIG_H  -DREDIS -DDEBUGMODE -DTXT_LOG  -DTEST  -I/usr/local/include -I. -g -O3 

LINFLAGS = -ldl -pthread $(CFLAGS)
LINFLAGS_CXX = -ldl -pthread $(CFLAGS_CXX)

MAC_SHARED = -flat_namespace -bundle -undefined suppress

OBJS = cyclequeue.o hashtable.o thread.o util.o daemon.o db_connect.o cgi_util.o redis_keepalive.o
#ltcmalloc_and_profiler

$(PROG): redisc json  $(OBJS) 
	#$(CC)  ais.c   $(OBJS)  -o $(PROG) $(LINFLAGS) -L/usr/local/lib -L/usr/local/lib -lsybdb -ltcmalloc_and_profiler -lhiredis -ljson  -lrt -levent  -luuid -lm -liconv
	$(CXX)  ais.c   $(OBJS)  -o $(PROG) $(LINFLAGS_CXX) -L/usr/local/lib -L/usr/local/lib -lsybdb -ltcmalloc_and_profiler -lhiredis -ljson  -lrt -levent  -luuid -lm -liconv -lstdc++


redisc:
	cd ./redisc && make install && cd ../
json:
	cd ./json && ./configure && make install && cd ../
#mxml:
#	cd ./mxml && ./configure && make install && cd ../
	
clean:
	rm -rf *.o core.* $(PROG) *.so
