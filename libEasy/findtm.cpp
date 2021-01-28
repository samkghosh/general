/*
* File Name: findtm.cpp
* Part of Easy Library
* Copyright @ 2001
*/

#include <afxwin.h>
#include <afxdisp.h>

#include <stdio.h>
#include <time.h>


#include "easy.h"
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



bool egParseDateTime(char *szdt, struct tm *ltm)
{
	COleDateTime	dt;
	
	if((szdt != NULL) && (szdt[0] != '\0'))
	{
		dt.ParseDateTime(szdt);
		if(dt.GetStatus() == COleDateTime::valid)
		{
			ltm->tm_mday = dt.GetDay();
			ltm->tm_mon = dt.GetMonth() - 1;
			ltm->tm_year = dt.GetYear() - 1900;
			ltm->tm_hour = dt.GetHour();
			ltm->tm_min = dt.GetMinute();
			ltm->tm_sec = dt.GetSecond();
			return true;
		}
	}

	return false;
}