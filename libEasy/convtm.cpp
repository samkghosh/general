/*
* Filename: convtm.cpp
* Pert of Easy Library
* Copyright @ 2001
*/
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <time.h>
#include <errno.h>
#include <memory.h>

#include "easy.h"




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



struct DateType
{
	char d1;
	char d2;
	char m1;
	char m2;
	char y1;
	char y2;
	char y3;
	char y4;
	char h1;
	char h2;
	char mi1;
	char mi2;
	char s1;
	char s2;
};

#define ORD(X) (X- '0')
#undef __FNNAME__
#define __FNNAME__ "egConvertTime"
unsigned long egConvertTime(char *dd)
{
	DateType d;
	struct tm mTime;
	unsigned long n;

	memset(&d, '0', sizeof(d));

	memcpy(&d, dd, (strlen(dd) -1));


		mTime.tm_sec = ( ORD(d.s1) *10 + ORD(d.s2)) % 60; //Seconds after minute (0 – 59)
		mTime.tm_min = ( ORD(d.mi1) * 10 + ORD(d.mi2)) % 60;	//Minutes after hour (0 – 59)
		mTime.tm_hour = ( ORD(d.h1)* 10 + ORD(d.h2)) % 24; //Hours after midnight (0 – 23)
		
		mTime.tm_mday = ( ORD(d.d1) * 10 + ORD(d.d2)) % 31; //Day of month (1 – 31)

		mTime.tm_mon = ( ORD(d.m1) *10 + (ORD(d.m2 )) -1) % 12; //Month (0 – 11; January = 0)

		mTime.tm_year = ( ORD(d.y1) * 1000 + ORD(d.y2) * 100 + ORD(d.y3) * 10 + ORD(d.y4))  - 1900; //Year (current year minus 1900)

//		tm_wday //Day of week (0 – 6; Sunday = 0)

//		tm_yday //Day of year (0 – 365; January 1 = 0)

		mTime.tm_isdst= 0;

		if ((n = mktime(&mTime)) == -1)
		{
			egerrno = errno;
			return -1;
		}

		return n;
}



#undef __FNNAME__
#define __FNNAME__ "egConvertTime"
unsigned long egConvertTimeGMT(char *dd)
{
//	DateType d;
//	struct tm *mTime;

	struct tm gtm;
	struct tm *tmpGtm;

	time_t n;

	if ( ( n = egConvertTime(dd)) == -1)
		return -1;

	tmpGtm = gmtime(&n);
	memcpy(&gtm, tmpGtm, sizeof(gtm));
	return(mktime(&gtm));
}




#if 1
/*
*
0 or 100	Default		mon dd yyyy hh:mm		[17]
1 101		USA			mm/dd/yyyy				[10] + [1] + [8] or [11] = [19] or [22]
1 102		ANSI		yyyy.mm.dd				[10] + [1] + [8] or [11] = [19] or [22]
3 103	British/French	dd/mm/yyyy				[10] + [1] + [8] or [11] = [19] or [22]
4 104		German		dd.mm.yyyy				[10] + [1] + [8] or [11] = [19] or [22]
5 105		Italian		dd-mm-yyyy				[10] + [1] + [8] or [11] = [19] or [22]
6 106		–			dd-mon-yyyy				[11] + [1] + [8] or [11] = [20] or [23]
7 107		–			mon.dd.yyy				[10] + [1] + [8] or [11] = [19] or [22]
– 8 or 108	–			hh:mm:ss				[8]			= [8]
– 9 or 109	–			mon dd yyyy hh:mi:ss:mmmAM (or PM)	[26]
	
10 110		USA			mm-dd-yy				[8] + [1] + [8] or [11] = [16] or [19]
11 111		JAPAN		yy/mm/dd				[8] + [1] + [8] or [11] = [16] or [19]
12 112		ISO			yymmdd					[6] + [1] + [8] or [11] = [14] or [17]
– 13 or 113 –			dd mon yyyy hh:mi:ss:mmm (24 h)	[24]
 
14 114		–			hh:mi:ss:mmm (24 h)		[11]
-------------------------------------------------
						yyyy-mm-dd
						Sun May 01 20:27:01 1994 [24]
						Mon, 23 Jul 2003 23:59:05 +0530 [31]

* Tokens are:
*	- week Day
*	- month
*	- day
*	- year
*	- hour
*	- min
*	- sec
*	- ms
*	- amPm
* Separators:
*	"-", "/",".", " ", ":"
*/

char *Ml[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
char *Ms[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static int __dateFormat__ = 0;

void egSetDateFormat(int x)
{
	if ( x == 1)
		__dateFormat__ = 1;
	else
		__dateFormat__ = 0;
}

#undef __FNNAME__
#define __FNNAME__ "egConvertTime"
unsigned long egMakeTime(char *dd)
{
	unsigned long	n;
	int i;
	char			szMon[16];
	char			szWd[8];
	int				year=1900;
	int				mon = 1;
	int				day = 1;
	int				hr =  0;
	int				min = 0;
	int				sec = 0;
	int				len;
	int a[3];
	char *pch;
	char xx[64];


	egStrip(dd, ' ');
	try
	{
		szMon[0] = '\0';
		szWd[0] = '\0';
		len = strlen(dd);

		switch(len)
		{
			case 9:
				//dd-mon-yy oracle date
				sscanf(dd,"%d-%s", &day, szMon); //,  &hr, &min);
				pch = strchr(szMon,'-');
				
				if ( pch != NULL)
				{
					*pch = '\0';
					pch++;
					year = atoi(pch);
				}

				for (i = 0; i < 12; i++)
				{
					if ( stricmp(szMon, Ms[i]) == 0 || stricmp(szMon, Ml[i]) == 0 )
					{
						mon = i+1;
						break;
					}
				}
				
				if ( i == 12 )
					return -1;
				break;
	
			case 17: /* default 100 */
				//mon dd yyyy hh:mm
				sscanf(dd,"%s %d %d %d:%d", szMon, &day, &year,  &hr, &min);

				for (i = 0; i < 12; i++)
				{
					if ( stricmp(szMon, Ms[i]) == 0 || stricmp(szMon, Ml[i]) == 0 )
					{
						mon = i+1;
						break;
					}
				}
				
				if ( i == 12 )
					return -1;

				break;

			case 22: /* 101,102,103,104,105,107 with 114 */
			case 19: /* 101,102,103,104,105,107  with 108*/
				pch = strchr(dd, ' ');
				if ( pch == NULL)
					return -1;
				pch++;
				sscanf(pch,"%d:%d:%d", &hr, &min, &sec);
				// now fall through

			case 10: /* 101,102,103,104,105, 107 */
					strcpy(xx,dd);
	
					pch = strtok(xx,".-/");

					if ( pch == NULL)
						return -1;

					a[0] = atoi(pch);
	
					pch = strtok(NULL,".-/");

					if ( pch == NULL)
						return -1;

					a[1] = atoi(pch);
			
					pch = strtok(NULL, ".-/");

					if ( pch == NULL)
						return -1;

					a[2] = atoi(pch);
				
					if ( a[0] > 1900 )
					{
						year = a[0];
						mon = a[1];
						day = a[2];

						if ( mon <= 0 && mon > 12)
							return -1;

						if ( day <= 0 && day > 31)
							return -1;
						break;
					}

					year = a[2];

					if ( a[0] > 12  && a[1] > 12 )
						return -1;

					if ( __dateFormat__ == 1)
					{

						if ( a[0] > 12 )
						{
							mon = a[1];
							day = a[0];
						}
						else
						{
							mon = a[0];
							day = a[1];
						}
					}
					else
					{
						if ( a[1] > 12 )
						{
							mon = a[0];
							day = a[1];
						}
						else
						{
							mon = a[1];
							day = a[0];
						}
					}
					break;
//		case 11:
//				break;

			case 8:  /* 108, 110, 111*/
			case 26: /* 109 */
			case 16: /* 110, 111 with 108 */
//		case 19: /* 110, 111 with 114 */
			case 6:  /* 112 */
			case 14: /* 112 with 108 */
//		case 17: /* 112 with 114 */

			case 11: /* 114 */ 
//		case 24: /* 113 */
			case 24: /* Asci time */ 
				if ( isalpha(dd[0]) == false)
				{
					// may be 113 format
					sscanf(dd,"%d %s %d %d:%d:%d", &day, szMon, &year,  &hr, &min, &sec);
				}
				else
				{
					//wd mon day hh:MM:ss yyyy
					sscanf(dd,"%s %s %d %d:%d:%d %d", szWd, szMon, &day,  &hr, &min, &sec, &year);
				}
		
				for (i = 0; i < 12; i++)
				{
					if ( stricmp(szMon, Ms[i]) == 0 || stricmp(szMon, Ml[i]) == 0 )
					{
						mon = i+1;
						break;
					}
				}
				
				if ( i == 12 )
					return -1;

				break;
			case 20:
				sscanf(dd,"%d %s %d %d:%d:%d", &day, szMon, &year, &hr, &min, &sec);
		//		sscanf(dd,"%s %d %s %d", szWd, &day, szMon, &year, &hr);
		
				for (i = 0; i < 12; i++)
				{
					if ( stricmp(szMon, Ms[i]) == 0 || stricmp(szMon, Ml[i]) == 0 )
					{
						mon = i+1;
						break;
					}
				}
				
				if ( i == 12 )
					return -1;
				break;

			case 31: /* RFC 822 date and time format */
					//	Mon, 23 Jul 2003 23:59:05 +0530 [31]
				sscanf(dd,"%s %d %s %d %d:%d:%d", szWd, &day, szMon, &year, &hr, &min, &sec);
		//		sscanf(dd,"%s %d %s %d", szWd, &day, szMon, &year, &hr);
		
				for (i = 0; i < 12; i++)
				{
					if ( stricmp(szMon, Ms[i]) == 0 || stricmp(szMon, Ml[i]) == 0 )
					{
						mon = i+1;
						break;
					}
				}
				
				if ( i == 12 )
					return -1;
				break;

			default:
				return -1;
		}


//long egConvertTime(int dd , int mm, int yy, int hh, int min, int ss)
	
		if ( year < 100)
			year = year+2000;

		if ( ( n = egConvertTime(day, mon, year, hr, min, sec)) == -1)
			return -1;

	
		return(n);
	}
	catch(...)
	{
		return -1;
	}
}


#endif
