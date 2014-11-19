#include "db_connect.h"
RETCODE db_connect(const char * user,const char * passwd, const char* serv_name,const char * db_name,DBPROCESS **db_proc)
{
	LOGINREC *login;
	DBPROCESS *dbproc;
	RETCODE erc=-1;
	if(dbinit()==FAIL)
		{
		fprintf(stderr,"%s:%d: dbinit() failed\n", __func__, __LINE__);
		return erc;
		}
	if((login=dblogin())==NULL)
		{
		
		fprintf(stderr, "%s:%d: unable to allocate login structure\n", __func__, __LINE__);
		return erc;
		}
	DBSETLUSER(login,user);
	DBSETLPWD(login,passwd);
	if((dbproc=dbopen(login,serv_name))==NULL)
		{
		 fprintf(stderr, "%s:%d: unable to connect to %s as %s\n", __func__, __LINE__,serv_name, user);
		return erc;
		}
	if(db_name && ((erc=dbuse(dbproc,db_name))==FAIL))
		{
		
		fprintf(stderr, "%s:%d: unable to use to database %s\n", __func__, __LINE__, db_name);
		return erc;
		}
	*db_proc=dbproc;
	return erc;
}
RETCODE dbExecSql(const char * sql_str,DBPROCESS **db_proc)
{
  RETCODE erc=-1;
  dbcmd(*db_proc,sql_str);
  if((erc=dbsqlexec(*db_proc))==FAIL)
  	{
  	fprintf(stderr, "%s:%d: exec sql:%s err\n", __func__, __LINE__,sql_str);
	return erc;
  	}
  return erc;
}


	

