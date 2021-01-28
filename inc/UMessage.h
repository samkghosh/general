
//
//////////////////////////////////////////////////////////////////////

#ifndef __UMESSAGE_H__
#define __UMESSAGE_H__

#include "Tcpip.h"


//#define MAXPKTSZ	1024
//#define SIZELEN		(sizeof(long))
#define SEND_SIZE 1625
int frame_len;
int sock_sender;
long frame_counter;
sockaddr_in  sender;

struct Frame
{
	long Frame_No;
	long DataSize;
	char buf[SEND_SIZE];
};


int SendBuffer( char szBuffer[], unsigned int iBufferSize );
int SendBufferTo( char *szSysName, int iPort, char *szBuffer, unsigned int iBufferSize );

#endif
