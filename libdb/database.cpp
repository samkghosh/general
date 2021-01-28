/********************************************
* FileName: database.h
*
********************************************/
#include <stdio.h>
#include <errmsg.h>
#include <stdlib.h>
#include "database.h"


#define CONNECT_TIME_OUT	10	// in days	

/*************************************************************************
*
*************************************************************************/
#undef __FNNAME__
#define __FNNAME__	"Database"
DataBase::DataBase()
{
	gdbMutex = PTHREAD_MUTEX_INITIALIZER ;
	bConnected = false;
	szErrorMsg[0] = '\0';
	szCon = NULL;
}




/**********************************************************************************
 * st connection to MySQL server at 'reading initial communication packet'
* Function Name: Connect
*
**********************************************************************************/
#undef __FNNAME__
#define __FNNAME__	"Connect"
bool DataBase::Connect(char *server,char *user, char* password,char* database)
{
	unsigned int uTimeOut;

//fprintf(stderr,"Going To connect DB=%s\n", server);
	if (server != NULL)
		strcpy(szServerIP, server);

	if (user != NULL)
		strcpy( szUID, user);

	if (password != NULL)
		strcpy(szPWD, password);
	
	if (database != NULL)
		strcpy(szDBName, database);

	if (bConnected == true)
	  mysql_close(szCon);

	szCon=mysql_init(NULL);


	my_bool bReConnect = true;

	 if (mysql_options(szCon, MYSQL_OPT_RECONNECT, (const char *)&bReConnect) != 0 )
	{
		fprintf(stderr,"**** UNABLE to SET RE-CONNECT\n");
	}
 	
	uTimeOut = 60 * 60 * 24 * CONNECT_TIME_OUT;

	 if (mysql_options(szCon, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&uTimeOut) != 0 )
	{
		fprintf(stderr,"**** UNABLE to SET CONNECT TIMEOUT \n");
	}

//fprintf(stderr,"Going To Call real connect DB=%s\n", server);
	if(!mysql_real_connect(szCon,server,user,password,database,0,NULL,0))
	{
		sprintf(szErrorMsg,"CONNECT:%s", mysql_error(szCon));
//fprintf(stderr,"%s\n", szErrorMsg);
		mysql_close(szCon);
        	return DB_ERROR;
	}


//fprintf(stderr,"connected DB=%s\n", server);

        bConnected = true;
 	return DB_SUCCESS;
}


/**********************************************************************************
* Function Name: CloseDB
*
**********************************************************************************/
#undef __FNNAME__
#define __FNNAME__	"CloseDB"
void DataBase::CloseDB()
{
	if ( szCon != NULL)
	{
		mysql_close(szCon);
		szCon = NULL;
	}
	return;
}

/****************************************************************
* Function Name: Fetch Record
*
*****************************************************************/
int DataBase::FetchRecord(char stmt[],_CArray2D *arry,int nMaxRecFetch,
									int* nTotalRec)
{
	int iRowCount = 0;
	int num_fields;
	
	MYSQL_RES *szRes;
	MYSQL_ROW szRow;
	int i;
	char szValue[256];

	/* [02-11-2016]	[Retry if there is error */
	int iRetry = 0;
	int rc = DB_SUCCESS;
	int iRow;

	lockdb();

	rc = mysql_query(szCon,stmt);
	if ( rc != 0 )
	{
		sprintf(szErrorMsg,"QUERY: %s", mysql_error(szCon));

		if ( rc == CR_SERVER_GONE_ERROR || rc == CR_SERVER_LOST)
		{
			if ( Connect(NULL,NULL,NULL,NULL) == false)
			{
				sprintf(szErrorMsg,"DBReOpenError");
				rc = DB_ERROR;
				goto GETOUT;
			}
			rc = mysql_query(szCon,stmt);
			if ( rc != 0)
			{
				sprintf(szErrorMsg,"QUERY: %s", mysql_error(szCon));
				rc = DB_ERROR;
				goto GETOUT;
			}
		}
		else
		{
			rc = DB_ERROR;
			goto GETOUT;	
		}
	}
	
        szRes=mysql_store_result(szCon);

//	szRes=mysql_use_result(szCon);
//printf("Rows=%d\n", mysql_num_rows(szRes));
        num_fields=mysql_num_fields(szRes);
//printf("num_fields=%d\n", num_fields);

	*nTotalRec =0 ;
	iRow = 1;
	while( (szRow = mysql_fetch_row(szRes)))
	{
		unsigned long *lengths;
		lengths = mysql_fetch_lengths(szRes);
	  	for( i=0; i < num_fields; i++)
		{
		
			sprintf(szValue,"%.*s", (int)lengths[i], szRow[i]?szRow[i]: "");
//printf("SzValue=%s\n",szValue);
			arry->add(iRow, i+1, szValue);
		}
		*nTotalRec = iRow;
		iRow++;
		if (iRow >nMaxRecFetch)
			break;
	}

	if ( szRes != NULL)
		mysql_free_result(szRes);

	rc = DB_SUCCESS;
GETOUT:

	unlockdb();
//fprintf(stderr,"%s\n", szErrorMsg);
	
	return rc;       
}




/***************************************************
Function: ExecuteQuery in Table
input:- tablename,stmt
return:- number of affected rows
****************************************************/
#undef __FNNAME__
#define __FNNAME__	"ExecuteQuery"
int DataBase:: ExecuteQuery(char *stmt)
{
	int rc;

	lockdb();
	rc = mysql_query(szCon,stmt);
	if ( rc != 0 )
	{
		if ( rc == CR_SERVER_GONE_ERROR || rc == CR_SERVER_LOST)
		{
			if ( Connect(NULL,NULL,NULL,NULL) == false)
			{
				rc = DB_ERROR;
				goto GETOUT;
			}
			rc = mysql_query(szCon,stmt);
			if ( rc != 0)
			{
				sprintf(szErrorMsg,"QUERY: %s", mysql_error(szCon));
				rc = DB_ERROR;
				goto GETOUT;
			}
		}
		else
		{

			sprintf(szErrorMsg,"QUERY: %s", mysql_error(szCon));
			rc = DB_ERROR;
			goto GETOUT;
		}
	}

	rc = (mysql_affected_rows(szCon));
GETOUT:
	unlockdb();

	return DB_SUCCESS;
}




/*****************************************************************\
 *lock scan table  
\****************************************************************/

int DataBase::lockdb()
{
  	int rc;
 	if((rc=pthread_mutex_lock(&gdbMutex))!=0)
  		return rc;
	else
 		return 0;

}


/**********************************************************
* FNNAME: unlockdb()
* Purpose: to unlock the serialisation
***********************************************************/
int DataBase::unlockdb()
{
   int rc;
  rc=pthread_mutex_unlock(&gdbMutex);

   return rc;
}


/**********************************************************
* FNNAME: releasedblock()
* Purpose: to release or destroy the lock handle
***********************************************************/
int DataBase::releasedblock()
{
  pthread_mutex_destroy(&gdbMutex);
  return 0;
}

#if 0
/***************************************************
Function: UpdateTable
input:- tablename,stmt
return:- number of affected rows
****************************************************/
int DataBase:: UpdateTable(char stmt[])
{
	mysql_query(szCon,stmt);
	return DB_SUCCESS;
}
#endif

/*********************************************************
* FNNAME: GetCount()
* Purpose: Get the record count with given query set. Return +ve
*		number if the record exits else returns 0
* [16-04-2015]	[SG]	
**********************************************************/
int DataBase::GetCount(char *pszQuery)
{
	int nTotalRec = 0;
	char szVal[32];
//	char arrayRec[1][MAX_DB_REC_LEN];
	_CArray2D arry;

	FetchRecord(pszQuery, &arry,1,&nTotalRec);

	if (nTotalRec <= 0)
		return 0;
//fprintf(stderr, "Record count=%s\n", arrayRec[0]);
	if ( arry.get(1,1, szVal) == NULL)
		return 0;
	else
		return atoi(szVal);
}


#if 0
/****************************************************************
* Function Name: Fetch Record
*
*****************************************************************/
T_DBRec * DataBase::FetchRecord(char stmt[],char arrayRec[][MAX_DB_REC_LEN],int nMaxRecFetch,
									int* nTotalRec)
{
	int iRowCount = 0;
	int num_fields;
	
	MYSQL_RES *szRes;
	MYSQL_ROW szRow;
	int i;



	lockdb();

	

	if(mysql_query(szCon,stmt))
	{
		sprintf(szErrorMsg,"QUERY: %s", mysql_error(szCon));
		unlockdb();
		return DB_ERROR;
	}
	
        szRes=mysql_store_result(szCon);

//	szRes=mysql_use_result(szCon);
        num_fields=mysql_num_fields(szRes);

	*nTotalRec = num_fields;


        if ( (szRow = mysql_fetch_row(szRes)) != NULL)
	{
	  	for( i=0; i < num_fields; i++)
		{
			if ( szRow[i] != NULL)
		   		strcpy(arrayRec[i],szRow[i]);
			else
				arrayRec[i][0] = '\0';
		}
	}

	if ( szRes != NULL)
		mysql_free_result(szRes);

	unlockdb();
//fprintf(stderr,"%s\n", szErrorMsg);
	
	return DB_SUCCESS;       
}
#endif
