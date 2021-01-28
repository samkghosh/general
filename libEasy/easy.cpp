// easy.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>


#include "easy.h"
int egerrno;


int makeDir(char *path)
{

	char *pCh;
	char currPath[128];
	char prevPath[256];
	char drive[10];
	int pLen;

    _splitpath(path,drive, NULL, NULL,NULL);


	pLen = 0;
	strcpy(prevPath,drive);
	pCh = path + strlen(drive);

	while (1)
	{
		if ( *pCh == '\\' || *pCh == '/')
		{
			if (pLen != 0)
			{
				currPath[pLen] = '\0';
				strcat(prevPath,currPath);
				if ( _mkdir(prevPath) == -1)
				{
					if (errno == ENOENT)
					{
						egerrno = _EG_NODIR;
						return -1;
					}
				}

//				strcat(prevPath,"\\");
				pLen =0;

			}

		}
		currPath[pLen] = *pCh;
		pLen++;
		pCh++;
		
		if ( *pCh == '\0')
		{
			currPath[pLen] = '\0';
			strcat(prevPath,currPath);
			if ( _mkdir(prevPath) == -1)
			{
				if (errno == ENOENT)
				{
					egerrno = _EG_NODIR;
					return -1;
				}
			}
			break;
		}
	}

	return 0;

}


int appendText(char *fname, char *buff)
{
	FILE * fp;


	if ( (fp = fopen(fname, "a+t")) == NULL)
	{
		egerrno = errno;

		return -1;
	}

	fputs(buff, fp);

	fclose(fp);

	return 0;
}




#if 0
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
		switch(flag)
		{
			case EG_SAMEDAY:
				if ( fabs((fInfo.time_access - n)) < (24*60*60))
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
		}

		if ( del == true )
		{
			if (fInfo.attrib & _A_SUBDIR)
				continue;
			else
			{
				sprintf(fname,"%s%s",path,fInfo.name);
				_unlink(fInfo.name);
				//printf("Delted %s\n", fname);
			}
		}

	}while(_findnext(fHandle,&fInfo) == 0);

	_findclose(fHandle);

	return 0;
}



/*
* Get all file names date < = >
*/

#undef __FNNAME__	
#define __FNNAME__	"delFiles"

int egGetFiles(EGFname_t *fNames, int no, char *fSpec, char *ddmmyyyy, int flag)
{
	_finddata_t fInfo;
	long fHandle;
	bool del;
	long n;
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
				if ( fabs((fInfo.time_access - n)) < (24*60*60))
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




/*
* checks the supplied time and sets the date to a long 
*/

long egCheckNsetTime(char *ddmmyyyy, int flag)
{
	struct tm mTime;
	time_t n;
	int m,d,y;

	egerrno = 0;
	// get the dates
	if ( ddmmyyyy == NULL )
	{
		// get the current time
		n = time(NULL);
		egerrno = 0;
	}
	else
	{
		egerrno = _EG_INVDATEFORMAT;

		if (strlen(ddmmyyyy) != 8 )
		{
			return -1;
		}

		d = (ddmmyyyy[0] - '0') * 10 + (ddmmyyyy[1] - '0');

		if ( d < 1 && d > 31)
			return -1;

		m = (ddmmyyyy[2] - '0') * 10 + (ddmmyyyy[3] - '0');

		if ( m < 1 && m > 12 )
			return -1;

		y = atoi(&ddmmyyyy[4]);

		if ( y < 1970 && y >2038 )
		{
			egerrno = _EG_YEAROUTOFRANGE;
			return -1;
		}

		egerrno = 0;

		if ( flag == EG_DAYSAFTER )
		{
			mTime.tm_sec = 59; //Seconds after minute (0 – 59)
			mTime.tm_min = 59;	//Minutes after hour (0 – 59)
			mTime.tm_hour = 23; //Hours after midnight (0 – 23)
		}
		else
		{
			mTime.tm_sec = 0; //Seconds after minute (0 – 59)
			mTime.tm_min = 0;	//Minutes after hour (0 – 59)
			mTime.tm_hour = 0; //Hours after midnight (0 – 23)			
		}

		mTime.tm_mday = d; //Day of month (1 – 31)

		mTime.tm_mon = m-1; //Month (0 – 11; January = 0)

		mTime.tm_year = y - 1900; //Year (current year minus 1900)

//		tm_wday //Day of week (0 – 6; Sunday = 0)

//		tm_yday //Day of year (0 – 365; January 1 = 0)

		mTime.tm_isdst= 0;

		if ((n = mktime(&mTime)) == -1)
		{
			egerrno = errno;
			return -1;
		}
	}
	return n;
}



/*
* Conver dd mm yy hh mm ss to a long time from Jan 1st 1970
*/

#undef __FNNAME__
#define __FNNAME__ "egConvertTime"
long egConvertTime(int dd , int mm, int yy, int hh, int min, int ss)
{
	struct tm mTime;
	time_t n;
//	int m,d,y;

	// get the dates

	egerrno = 0;

	if ( yy == 0 )
	{
		// get the current time
		n = time(NULL);
		egerrno = 0;
	}
	else
	{
		egerrno = _EG_INVDATEFORMAT;

		if ( dd < 1 && dd > 31)
			return -1;

		if ( mm < 1 && mm > 12 )
			return -1;

		if ( yy < 1970 && yy >2038 )
		{
			egerrno = _EG_YEAROUTOFRANGE;
			return -1;
		}

		if ( ss < 0 && ss > 59 )
			return -1;

		if ( min < 0 && min > 59 )
			return -1;

		
		if ( hh < 0 && hh > 23 )
			return -1;

		
		egerrno = 0;

		mTime.tm_sec = ss; //Seconds after minute (0 – 59)
		mTime.tm_min = min;	//Minutes after hour (0 – 59)
		mTime.tm_hour = hh; //Hours after midnight (0 – 23)
		
		mTime.tm_mday = dd; //Day of month (1 – 31)

		mTime.tm_mon = mm-1; //Month (0 – 11; January = 0)

		mTime.tm_year = yy - 1900; //Year (current year minus 1900)

//		tm_wday //Day of week (0 – 6; Sunday = 0)

//		tm_yday //Day of year (0 – 365; January 1 = 0)

		mTime.tm_isdst= 0;

		if ((n = mktime(&mTime)) == -1)
		{
			egerrno = errno;
			return -1;
		}
	}
	return n;
}



/*
* egAppendText appends the text to a file. If file is not present,
* it creates the file
* This can be called from Multi threaded application
*/


int egAppendText(char *fName, char *buff)
{

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
							egerrno = GetLastError();
							return -1;
						}
					}
			}
				

            __finally { 
                // Release ownership of the mutex object.
                if (! ReleaseMutex(mHandle)) {
					egerrno = GetLastError();
                   return -1;
                } 

            break; 
        } 

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

#endif




#if 0
/*
*
*/
/*
* Copy function
*/

int egCopyFile(char *source, char *dest)
{

		FILE *sFp;
	FILE *dFp;
	unsigned ret;
	char buffer[4*1024];


	if ( (sFp = fopen(source, "r+b")) == NULL)
	{
#ifdef DEBUG
		TRACE("File Open Failed %s [%d]\n", source, errno);
#endif
		return -1;
	}

	if ( (dFp = fopen(dest, "w+b")) == NULL)
	{
#ifdef DEBUG

		TRACE("File Open Failed %s [%d]\n", dest, errno);
#endif

		fclose(sFp);
		return -1;
	}


	do
	{
		 ret = fread(buffer,1, sizeof(buffer), sFp);
		 
		if (ferror(sFp) )
		{
			fclose(dFp);
			_unlink(dest);
			fclose(sFp);
#ifdef DEBUG

			TRACE("Read Error %d\n", errno);
#endif
			return -1;
		}

		// write content
		if ( ret > 0 )
		  if (fwrite(buffer,1, ret,dFp) != ret )
		  {
			fclose(dFp);
			_unlink(dest);
			fclose(sFp);
#ifdef DEBUG
			TRACE("Write Error %d\n", errno);
#endif
			return -1;
		  }
	}while(!feof(sFp));

	fclose(sFp);

	fclose(dFp);

	return 0;

}

/*
*
*/

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
		return 0;
}


/*
*
*/

int egCheckPath(char *pathName, bool creatIt)
{
	struct _stat stBuff;


	// check the existance of the
	if ( _stat(pathName, &stBuff) == -1)
	{
		if ( errno == ENOENT )
		{
			if ( creatIt == true)
				return( makeDir(pathName));
		}
		
		egerrno = errno;
		return -1;
			
	}
	
	if ( stBuff.st_mode & _S_IFDIR )
	{
		return 0;
	}
	else
	{
		egerrno = _EG_NODIR;
		return -1;
	}

}



/*
*
*/

int egFindTime(int fhhmm, int thhmm, int cur)
{
#define F (24 * 60)
	int f,t,n;

	time_t ct;
	struct tm *ctm;


	if ( cur == -1)
	{
		ct = time(NULL);
		ctm = localtime(&ct);
		n = ctm->tm_hour * 60 + ctm->tm_min;
	}
	else
		n= ((((int)(cur / 100)) * 60 )+ ( cur % 100))% F;
    
	f = ((((int)(fhhmm / 100)) * 60 )+ ( fhhmm % 100)) % F;
    t = ((((int)(thhmm / 100)) * 60 )+ ( thhmm % 100)) % F;


	 if ( f == 0 && t == 0 )
		 return 0;

	 if ( f <= t)
	 {
		if ( n >= f && n <= t)
			return 0;
		else
		{
			if  (n < F && n > t)
				return((F - n)+(f));
			else
				return (f - n);
		}
	 }
	 else
	 {
		if ( n > t && n < f)
			return (f-n);
		else
			return 0;

	 }

	 return 0;
}


#endif

