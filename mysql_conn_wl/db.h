#ifndef DB_H
#define DB_H


#ifdef __cplusplus
extern "C" {
#endif

#include "vlsa_common.h"


// 数据库连接信息
typedef struct db_config
{
	char host[64];
	char user[32];
	char pwd[32];
	char dbname[32];
	short port;

}db_config_t;


typedef enum data_type
{
	INT_TYPE,
	LONGLONG_TYPE,	
	STR_TYPE,
	//TEXT_TYPE,
	
}data_type_e;


typedef struct data_fmt
{
	data_type_e type;
	void *pvalue;
	int len;			//pvalue缓冲区的长度，type = STR_TYPE时需指定!!
	
}data_fmt_t;


//return:RET_OK/RET_ERR
int db_init(db_config_t *db_cfg,int max_connection_num);

void db_end();


//out:	paffect_rows: output the affected  rows
//return:RET_OK/RET_ERR/RET_OTHER(no data affected)
int db_update_insert_del(char *sql,int *paffect_rows);


//in:	pdb: point to buf to save return data, pdb[row][col]
//		rownum: row of select data
//		col:	colum of select data
//out:	pretrow: return the real rows of select data
//return:RET_OK/RET_ERR/RET_OTHER(not found)
int db_select(char *sql,data_fmt_t *pdb,int rownum,int col,int *pretrows);



#ifdef __cplusplus
}
#endif


#endif

