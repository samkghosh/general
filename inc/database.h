/*******************************************************
* FileName: database.h
*
* layer to MySQl
********************************************************/
#ifndef __DATABASE_H__
#define __DATABASE_H__


#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <mysql.h>
#include <stdlib.h>

#define MAX_DB_REC_LEN	128

#define DB_SUCCESS	0
#define DB_ERROR	-1
#define DB_NOTCONNECT	-2
#define DB_OUTOF_SYNC	-3
#define DB_ERRORUNKNOWN	-4

enum
{
	DBTYPE_UNKNOWM,
	DBTYPE_INT,
	DBTYPE_LONG,
	DBTYPE_STRING
};


#define MAXCOL	50

typedef struct _ARRAY_
{
	char *pCol[MAXCOL];
	struct _ARRAY_	*next=NULL;
}T_ARRAY2D;


class _CArray2D
{
	T_ARRAY2D	*pArray2D=NULL;
	T_ARRAY2D	*pTmpNode=NULL;
	T_ARRAY2D	*pTail=NULL;

	public:
		_CArray2D()
		{
			pArray2D = new (T_ARRAY2D);
			for(int i = 0; i < MAXCOL; i++)
				pArray2D->pCol[i] = NULL;
			pArray2D->next= NULL;
			
		};

		~_CArray2D()
		{
			ReleaseAll();
		};

		void ReleaseAll()
		{
			pTmpNode = pArray2D;
			pTail = pArray2D;
			while(pTmpNode != NULL)
			{
				for(int i = 0; i < MAXCOL; i++)
				{
					if ( pTmpNode->pCol[i] != NULL)
						free(pTmpNode->pCol[i]);
				}
				pTmpNode = pTmpNode->next;
				if ( pTail != NULL)
					delete pTail;
				pTail = pTmpNode;
			}
		}
	
		void add(int iRow, int iCol, char *pszVal)
		{
			T_ARRAY2D *pTail;
			
			pTail  = pArray2D;
			if ( iCol > MAXCOL)
				return;

			for(int i = 1; i < iRow; i++)
			{	
				if ( pTail->next == NULL)
				{
					pTmpNode = new T_ARRAY2D;				
					memset(pTmpNode, '\0', sizeof(T_ARRAY2D));
					pTail->next = pTmpNode;
					pTail = pTail->next;
				}
				else
				{
					pTail = pTail->next;
					if ( pTail == NULL)
					{
						pTmpNode = new T_ARRAY2D;				
						memset(pTmpNode, '\0', sizeof(T_ARRAY2D));
						pTail = pTmpNode;
					}
				}
			}

			pTail->pCol[iCol-1]	= (char *)malloc(strlen(pszVal)+1);
			strcpy(pTail->pCol[iCol-1], pszVal);
		}

		char *get(int iRow, int iCol, char *pszVal)
		{
			T_ARRAY2D *pTail;
			pTail  = pArray2D;
			if ( iCol > MAXCOL)
				return NULL;

			if ( pTail == NULL)
				return NULL;

			for(int i = 1; i < iRow; i++)
			{	
	
				pTail = pTail->next;
				if (pTail == NULL)
					return NULL;
			}

			if ( pTail->pCol[iCol-1] != NULL)
				strcpy(pszVal, pTail->pCol[iCol-1]);
			else
				return NULL;
			return pTail->pCol[iCol-1];
		}

		char *get(int iRow, int iCol)
		{
			char szVal[256];
			return get(iRow, iCol, szVal);
		}

};


class DataBase
{
	MYSQL *szCon;
	bool bConnected;
	char szServerIP[256];
	char szUID[256];
	char szPWD[256];
	char szDBName[256];
	pthread_mutex_t gdbMutex; // = PTHREAD_MUTEX_INITIALIZER ;
 public:
	MYSQL_RES *szRes;
	MYSQL_ROW szRow;
	int       Ret;
       int      szResult;
       int      num_fields;        
	char     szErrorMsg[512];

	DataBase();
        bool Connect(char *server,char *user,char *passwd,char* database);        
        int  FetchRecord(char stmt[],_CArray2D *recArray, int nMaxRecFetch,int* nTotoalRec);
	int  ExecuteQuery(char stmt[]);
 	int lockdb();
	int unlockdb();	
	int releasedblock();	

	/* [16-04-2015]	[SG]	function added to get the record count*/
	int GetCount(char *pszQry);
	/* TBD::
       int ExecSelectQuery(
					CString			strQuery,
					CStringArray	*arrFields,
					int				nMaxRecordsFetch,
					int				*nRecordsFetched,
					bool			bDoLock=true);
	*/
	/* [02-11-2016]	[SG]	*/
	void CloseDB();

};


int lockdb(int iTimeOut);
int unlockdb();
int releasedblock();


#endif
