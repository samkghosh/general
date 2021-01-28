// Message.h: interface for the Message class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "Tcpip.h"


class Message  
{
private:
	char buff[MAXPKTSZ + MAXPKTSZ];
	char *currPtr;
	int totalCount;
   char szError[128];
   int fd;

   int readData(char *, unsigned int);
public:
	Message(int);
	virtual ~Message();
	int recvPkt( char *);
	int recvPkt(char *msg, int *type);

	int sendPkt(char *, unsigned int);
   	int sendPkt(char *, int);
	int sendPkt(char *, int type, int len );
	int sendPktRaw(char *, int);
	int recvPktRaw(char *msg, int len);


	int closeClient();

};


#define MAXMSGSIZE  (MAXPKTSZ - (sizeof(int) + SIZELEN))
struct GenMsg
{
   int type;
   unsigned char body[MAXMSGSIZE];
};

struct GenMsgU
{
/* [21-04-2015]	[SG]	*/
#ifdef _64BIT
	unsigned int type;
#else
   unsigned long type;
#endif
   unsigned char body[MAXMSGSIZE];
};


int setRcvTimeOut(int fd, unsigned int timeout);
int setKeepAlive(int fd, int v);

int sndMsgTo(char *ipAddr, int portNum, void * msg, int len, int type);

#define MSGSIZE(x)		(sizeof(int) + sizeof(x))
#endif 
