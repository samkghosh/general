// egLm.cpp : Defines the entry point for the DLL application.
//

#ifdef WIN32
#include "stdafx.h"
#endif
#include <string.h>
#include "egLm.h"
#ifdef WIN32
#include <io.h>
#endif

#include "eglmsdkl.h"
#include "eglmsdk.h"
#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	char lmFileName[256];

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
#if 1
			lmFileName[0] = '\0';	
			egLMgetLMDataFlName(lmFileName);
				
			egInitLm(lmFileName);
			break;
#endif
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// This is an example of an exported variable
EGLM_API int nEgLm=0;

// This is an example of an exported function.
EGLM_API int fnEgLm(void)
{
	return 42;
}

#endif

// This is the constructor of a class that has been exported.
// see egLm.h for the class definition
CEgLm::CEgLm()
{
	return; 
}

CEgLm::CEgLm(char *p)
{
	char lmFileName[256];


	strcpy(productId, p);

	lmFileName[0] = '\0';	

	egLMgetLMDataFlName(lmFileName);
				
	egInitLm(lmFileName);
	return; 
}



/********************************************************
*
***			return EGLM_INVALID;
** 			return EGLM_NOENTRYFOUND;
			return EGLM_TIMEEXPIRED;
			EGLM_OK
*******************************************************/
int CEgLm::check()
{
	return egLMCheck(productId);
}



/*************************************************************
*
			return EGLM_NOENTRYFOUND;
			return EGLM_OK;
			return EGLM_INVALID;
*************************************************************/
int CEgLm::getToken(char *tokenName, char * tokenValue)
{
	return egGetToken(productId, tokenName, tokenValue);
}


/*
* NULL means no version number found or no product found
*/
char * CEgLm::getVersion()
{
	return egGetVersion(productId);
}


/*
* 0: unlimited user
*/
int CEgLm::getCount()
{
	return egGetCount(productId);
}

