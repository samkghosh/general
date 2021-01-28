/*
* File Name: getfiles.cpp
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




#undef __FNNAME__	
#define __FNNAME__	"delFiles"

int egGetFiles(EGFname_t *fNames, int no, char *fSpec, char *ddmmyyyy, int flag)
{
	_finddata_t fInfo;
	long fHandle;
	bool del;
	time_t n;
	int count = 0;
	EGFname_t *ufile = fNames;


	egerrno = 0;
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
		switch(flag)
		{
			case EG_SAMEDAY:
				if ( fabs(double(fInfo.time_access - n)) < (24*60*60))
					del = true;
				break;
			case EG_DAYSAFTER:
				if ( fInfo.time_access > n)
					del = true;
				break;
			case EG_DAYSBEFORE:
					if ( fInfo.time_access < n)
					del = true;
				break;

			case EG_FORALLDAYS:
				del = true;
				
		}

		if ( del == true )
		{
			if ( count > no )
				break;
			strcpy((char *)ufile,fInfo.name);
			count++;
			ufile++;
		}
		
	}while(_findnext(fHandle,&fInfo) == 0);

	_findclose(fHandle);

	return count;
}


