
/***********************************************************
* File Name: konferLm.cpp
*
* Purpose: Checks the license for KonferLite
*
**********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "egLm.h"

CEgLm		gLicense("KonferLite");


/****************************************************************************************************
* FuncName: checkLicense
*
*****************************************************************************************************/
#undef __FNNAME__
#define __FNNAME__	"checkLicense"
void checkLicense(void *p)
{
	char szError[255];
	int iLicenseChanCount=8;
	int iErrNo;
	int  iChanNum;	
	int iLogInfo;
	FILE *fp;

	try
	{
		int rc = gLicense.check();
		switch(rc)
		{
			case EGLM_INVALID:
				sprintf(szError,"[WARNING] License is not valid. Signature or Host id is not correct");
				break;
			case EGLM_NOENTRYFOUND:
				sprintf(szError,"[WARNING] No License entry found for this product");	
				break;
			case EGLM_TIMEEXPIRED:
				sprintf(szError,"[WARNING] Time has expired");
				break;
		}

		if ( rc != EGLM_OK)
		{
			fp = fopen("/tmp/__KonferLite", "a+t");
			
                        if ( fp != NULL)
			{
				fprintf(stderr,"KonferLite(TM) License Warning [%s]", szError);
				fclose(fp);
			}
			exit(0);
		}

       }
       catch(...)
       {

		fp = fopen("/tmp/__KonferLite", "a+t");
		
                if ( fp != NULL)
		{
			fprintf(stderr,"KonferLite(TM) License Warning [%s]", szError);
			fclose(fp);
		}
		exit(0);
       }
       return;
}	
