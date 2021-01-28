/*
*File Name: chksettm.cpp
* A part of Easy Library
* Copyright @ 2001
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

