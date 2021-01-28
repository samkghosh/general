// easy.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
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



/*
* Deletes file of specified type from specified date < = >
*/

#undef __FNNAME__	
#define __FNNAME__	"egDelFiles"

int egDelFiles(char *fSpec, char *ddmmyyyy, int flag)
{
	_finddata_t fInfo;

	char drive[32];
	char dir[256];
	char path[256];
	char fname[256];

	long fHandle;
	bool del;
	long n;


	drive[0]='\0';
	path[0]='\0';
	egerrno = 0;

	_splitpath(fSpec,drive,dir,NULL,NULL);

	sprintf(path,"%s%s", drive, dir);

	if ((n = egCheckNsetTime(ddmmyyyy, flag)) == -1)
		return -1;


	if (( fHandle = _findfirst(fSpec, &fInfo)) == -1)
	{
		egerrno = errno;
		return -1;
	}

	do
	{
		if ((strcmp(fInfo.name,".") == 0) || (strcmp(fInfo.name, "..") == 0))
			continue;

		del = false;

//MODIFICATIONS
//Author:		Maneesh Jolly
//Date	:		24-September-2004
//Reason:		Bug fixing
//Description:	The variable time_access of structure _finddata_t was used to compare the times.
//				But the time given as function parameter should be compared to time when the file
//				was modified. so in the switch statement "fInfo.time_access" is replaced by "fInfo.time_write" 
		switch(flag)
		{
			case EG_SAMEDAY:
				//Earlier statement = if ( fabs((fInfo.time_access - n)) < (24*60*60))
				if ( fabs((double)(fInfo.time_write - n)) < (24*60*60))
					del = true;
				break;
			case EG_DAYSAFTER:
				//Earlier statement = if ( fInfo.time_access > n)
				if ( fInfo.time_write > n)
					del = true;
				break;
			case EG_DAYSBEFORE:
				//Earlier statement = if ( fInfo.time_access < n)
				if ( fInfo.time_write < n)
					del = true;
				break;
		}

		if ( del == true )
		{
			if (fInfo.attrib & _A_SUBDIR)
				continue;
			else
			{
				sprintf(fname,"%s%s",path,fInfo.name);

				_unlink(fname);
				//rc = DeleteFile(fname			
				//printf("Delted %s\n", fname);
			}
		}

	}while(_findnext(fHandle,&fInfo) == 0);

	_findclose(fHandle);

	return 0;
}



