#include "stdio.h"
#include "string.h"
#include "mysql.h"

class DataBase
{
public:
	MYSQL *szCon;
	MYSQL_RES *szRes;
	MYSQL_ROW szRow;
	int       Ret;
public:
	bool _ConnectMysql(char *server,char *user,char *passwd,char* database);        int  ExecuteQuery(char stmt[]);
	int  UpdateTable(char stmt[]);
	
};

