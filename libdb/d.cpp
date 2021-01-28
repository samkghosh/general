/*******************************************************
* FileName: database.cpp
* 
*
******************************************************/

#include "database.h"



/***************************************************
*
****************************************************/
#undef __FNNAME__
#define __FNNAME__	"_connectMySql"
bool DataBase::_ConnectMysql(char *server,char *user,
				char* password,char* database)
{

	szCon = mysql_init(NULL);
	if(!mysql_real_connect(szCon,server,user,password,database,0,NULL,0))
	{
		fprintf(stderr,"%s\n",mysql_error(szCon));
    return false;
	}

 
  return true;
}



/***************************************************
*
****************************************************/
#undef __FNNAME__
#define __FNNAME__	"ExecuteQuery"
in DataBase:: ExecuteQuery(char stmt[],char arrayRec[][30],int nMaxRecFetch,int* nTotalRec)
{

	if (mysql_query(szCon,stmt))
	{
		fprintf(stderr,"%s\n",mysql_error(szCon));
		return -1;
	}
	
        szRes = mysql_store_result(szCon);
//	szRes=mysql_use_result(szCon);
        num_fields = mysql_num_fields(szRes);

       *nTotalRec=num_fields;

	while ( (szRow=mysql_fetch_row(szRes)) )
	{
	  	for(int i=0;i<nMaxRecFetch;i++)
	  	{
			if ( szRow[i] != NULL)
		   		strcpy(arrayRec[i],szRow[i]);
			else
				arrayRec[i][0] = '\0';
	  
		}
	}
        
	return 0;       
}

/***************************************************
Function: UpdateTable
input:- tablename,stmt
return:- number of affected rows
****************************************************/
