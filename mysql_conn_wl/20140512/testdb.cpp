#include <iostream>
#include <cstdio>
using namespace std;


int main(int argc, int *argv[])
{
	#if 0
	typedef struct db_config
{
	char host[64];
	char user[32];
	char pwd[32];
	char dbname[32];
	short port;

}db_config_t;
	#endif
		db_config_t dbct={"127.0.0.1","root","root","test",3306 };
		
		db_init(&dbct,10 );
		fprintf(stderr,"---------  db_init finished  -----------");
		
		int roweffective;
		int ret= db_update_insert_del(" insert into test_tb (name, attr) values ('id3','id3attr'); ", &roweffective);
		fprintf(stderr,"---------  ret=%d  -----------",ret);
		fprintf(stderr,"---------  db_update_insert_del finished  -----------");
		
		db_end();
		fprintf(stderr,"---------  db_end   finished  -----------");
		
}


