#define DB_REC_LEN 128
#define MAXCOL	50

typedef struct _ARRAY_
{
	char *pCol[MAXCOL];
	struct _ARRAY_	*next;
}T_ARRAY2D;


class _CArray2D
{
	T_ARRAY2D	*pArray2D;
	T_ARRAY2D	*pTmpNode;
	T_ARRAY2D	*pTail;

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
			return pszVal;
		}
};
