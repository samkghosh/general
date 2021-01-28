/*
* Filename apendtxt.xpp
* Part of Easy Library
* Copyright @ 2001
*/


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>


#include "easy.h"

#include "easyl.h"
/*
* egAppendText appends the text to a file. If file is not present,
* it creates the file
* This can be called from Multi threaded application
*/


int egAppendText(char *fName, char *buff)
{
#if 0
	// create a mutex

	HANDLE mHandle;
	DWORD  dwWaitResult;
	char key[256];
	char *pCh;

	strcpy(key, fName);

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
        60000L);   // one 60-second time-out interval


    switch (dwWaitResult) 
    {
        // The thread got mutex ownership.
        case WAIT_OBJECT_0: 
            __try { 
                // Write to the database.
					if ( appendText(fName, buff) == -1 )
					{
						egerrno = errno;
					// Release ownership of the mutex object.
						if (! ReleaseMutex(mHandle)) 
						{ 
							CloseHandle(mHandle);
							egerrno = GetLastError();
							return -1;
						}
					}
			}
				

            __finally { 
                // Release ownership of the mutex object.
                if (! ReleaseMutex(mHandle)) {
					CloseHandle(mHandle);
					egerrno = GetLastError();
                   return -1;
                } 

            break; 
        } 

        // Cannot get mutex ownership due to time-out.
        case WAIT_TIMEOUT: 
			egerrno = GetLastError();
			CloseHandle(mHandle);

            return -1; 

        // Got ownership of the abandoned mutex object.
        case WAIT_ABANDONED: 
			ReleaseMutex(mHandle);
			egerrno = GetLastError();

            return -1; 
    }

	CloseHandle(mHandle);

	return 0;
#else
return -1;
#endif

}


