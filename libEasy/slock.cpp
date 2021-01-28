/*
* Filename: slock.cpp
* implements single lock of mutex
* part of easy library
* Copyright @2001
*/

#include <stdio.h>
#include <string.h>
#include <io.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <direct.h>

#include "easy.h"


int egLock(char *str, int timeout)
{

	// create a mutex

	HANDLE mHandle;
	DWORD  dwWaitResult;
	char key[256];
	char *pCh;

	strcpy(key, str);

	pCh = key;

	while ( *pCh != '\0')
	{
		if ((*pCh== ':') || (*pCh== '\\') || (*pCh == '.'))
			*pCh = '$';
		pCh++;
	}


	
	mHandle = CreateMutex(NULL,false, key);
//	mHandle = CreateMutex(NULL,false, NULL);

	if ( mHandle == NULL )
	{
		egerrno = GetLastError();
		return -1;
	}


    // Request ownership of mutex.

    dwWaitResult = WaitForSingleObject( 
        mHandle,   // handle to mutex
        long(timeout));   // one 60-second time-out interval


    switch (dwWaitResult) 
    {
        // The thread got mutex ownership.
        case WAIT_OBJECT_0: 
          		return (int)mHandle;
         
        // Cannot get mutex ownership due to time-out.
        case WAIT_TIMEOUT: 
			egerrno = GetLastError();;
            return -1; 

        // Got ownership of the abandoned mutex object.
        case WAIT_ABANDONED: 
			egerrno = GetLastError();
            return -1; 
    }

	return 0;
}

/*
*
*/

int egUnlock(int mHandle)
{
	if (! ReleaseMutex((void *)mHandle)) 
	{ 
		egerrno = GetLastError();
		return -1;
	}
	else
	{
		CloseHandle((void *)mHandle);
		return 0;
	}
}
