/*
*
* File Name: utility.h
*/

#ifndef __EASY_H__
#define __EASY_H__


#include <windows.h>
#include <mmsystem.h>        //multimedia header file
#include <mmreg.h>		    // Multimedia registration
#include <msacm.h>	


#define EG_SAMEDAY		0	// Operation for Same day
#define EG_DAYSBEFORE	-1	// Operation from day before
#define EG_DAYSAFTER	1	// Operation from Day after
#define EG_FORALLDAYS	2

#define _EG_INVDATEFORMAT	0x4000		// invalid data format
#define _EG_YEAROUTOFRANGE	0x4001		// Year given is out of range
#define _EG_LOCKFAILED		0x4002		// Locking faild
#define _EG_BADFILE			0x4003		// File is of invalid type
#define _EG_MEMERR			0x4004		// Memory allocation error
#define _EG_TIMEOUT			0x4005		// Time out has ocurred
#define _EG_SOCKSTARTFAIL	0x4006		// Opening and initializing of Socket has failed
#define _EG_NODIR			0x4007		// It is not a directory
#define	_EG_NOWAVEDRIVER	0x4008
#define _EG_WRITEERROR		0x4009

extern egerrno;


typedef char EGFname_t[128]; // Filename size

typedef struct 
{
	int waveFormat;		// wave format 
	int sampleRate;		// Sample Rate
	int bitsPerSample;	// Bits per Sample
	int size;			// size of the data chunk in the wave file
}EGWaveStat_t;



/*
* Prototypes
*/

// Append null terminated text array to the file 
int egAppendText(char *fName, char *buff);

// Appends two wave files of same format
int egAppendWave(char *dest, char * src);

// Copy a file
int egCopyFile(char *source, char *dest);

// checks given ddmmyyyy and sets to a long offset value form 1970 Jan 1
long egCheckNsetTime(char *ddmmyyyy, int flag = EG_SAMEDAY);


// checks and creates the given path. To create it make createIt to true
int egCheckPath(char *path, bool createIt = false);
   
// checks the value lies in a set 
bool egCheckValue(char * va, char * range);
bool egCheckValue(long va, char * range);

// converts 8 bit 8KHz, PCM to 1 bit 8Khz Mono
int egConvertTo1Bit( char* source_file,char* des_file);


// converts a dd, mm, yyyy to a long time offset from 1970 jan 1
long egConvertTime(int dd, int mm, int yyyy, int hh = 0, int min = 0, int ss=0); 
unsigned long egConvertTime(char *ddmmyyyyhhmmss);
unsigned long egConvertTimeGMT(char *dd);

// delets files for a date specified date(s)
int egDelFiles(char *fSpec, char *ddmmyyyy = NULL, int flag = EG_SAMEDAY);

// checks if the time falls in between two time (time is in HHMM format), returns time in mins
int egFindTime(int fhhmm, int thhmm, int cur = -1);

// find files for a specified date(s)
int egGetFiles(EGFname_t *fNames, int no, char *fSpec="*.*", char *ddmmyyyy=NULL, int flag = EG_FORALLDAYS);

// Gets the status of .WAV format file
int egGetWaveStat(char *fileName, EGWaveStat_t *);

// locks
int egLock(char *str, int timeout);

// make time
unsigned long egMakeTime(char *dd);

// Md5 Hash
int egMd5Hash(char *input, unsigned char *output);
int egMd5Hash(char *input, char *output);

// checks if a remote IP/Host is alive or responding
int egRemoteAlive(char *hostId, int timeout);

// Strips the supplied character from both sides of a String
char * egStrip(char * str, char chr= ' ');

// Strips the supplied character from left side of a String
char * egStripl(char * str, char chr= ' ');

// Strips the supplied character from right side of a String
char * egStripr(char * str, char chr= ' ');

// A thread safe strtok function
char * egStrtok(char *pStr, char *token, char * sep);

// unlocks a lock locked by egLock
int egUnlock(int mHandle);


bool egParseDateTime(char *szdt, struct tm *t);

#endif