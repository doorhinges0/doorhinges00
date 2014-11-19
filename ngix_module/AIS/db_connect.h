#ifndef  _DB_CONNENT
#define _DB_CONNENT
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sqlfront.h>
#include <sybdb.h>
RETCODE db_connect(const char * user,const char * passwd, const char* serv_name,const char * db_name,DBPROCESS **db_proc);
RETCODE dbExecSql(const char * sql_str,DBPROCESS **db_proc);


#endif

