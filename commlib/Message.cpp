// Message.cpp: implementation of the Message class.
//
//////////////////////////////////////////////////////////////////////
#include "Message.h"
#include <string.h>
#include <memory.h>
#include <stdio.h>

#undef TESTING
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define u_long	unsigned int
#define long	int

Message::Message(int fd1)
{
   fd = fd1;
   totalCount =0;
   currPtr = buff;

    int optVal = 1;
  // set socket option
	 setsockopt(
					 fd,                 
					SOL_SOCKET,                
					SO_KEEPALIVE,              
					(char  *)&optVal,
					sizeof(int));
	
	optVal = 2 * MAXPKTSZ;

	setsockopt( 
        fd, 
        SOL_SOCKET, 
        SO_RCVBUF, 
		(char *)&optVal,
        sizeof(int));

	setsockopt( 
        fd, 
        SOL_SOCKET, 
        SO_SNDBUF, 
		(char  *)&optVal,
        sizeof(int));

}

Message::~Message()
{
//  closesocket(fd);
}

#if 0
int Message::readData(char *tmpBuff, unsigned int size)
{
 
  int iReturn;
  unsigned char x[MAXPKTSZ];
  
  
  iReturn = recv (fd, tmpBuff, size, 0);

 if (iReturn == SOCKET_ERROR)
    {
	 	perror("recv()");
		 return SOCKET_TIMEOUT;

#ifdef TESTING
     	 fprintf(stderr, "No data is received, recv failed.");
                
#endif

      return SOCKET_ERROR;
    }
    else if (iReturn == 0)
    {
 //     printf("Finished receiving data,"Server MB_OK");
      return SOCKET_CLOSED;
    }
	else
	{
		memcpy(x, tmpBuff, iReturn);

        return iReturn;
	}
}


#else


int Message::readData(char *tmpBuff, unsigned int size)
{
 
  int iReturn;
  char x[MAXPKTSZ];
  int idx=0;
  int sizeLeft = size;
  
  while ( idx < size)
  {

	  iReturn = recv (fd,  x, sizeLeft, 0);

	  if (iReturn == SOCKET_ERROR)
	  {
	 		perror("recv()");
			return SOCKET_TIMEOUT;	
	     
		return SOCKET_ERROR;
	  }
	  else if (iReturn == 0)
      {
	      return SOCKET_CLOSED;
	  }
	  else
	  {

		memcpy((tmpBuff+idx), x, iReturn);
		idx = idx+iReturn;
		
		sizeLeft = sizeLeft - iReturn;
		
        if ( sizeLeft <= 0 )
			return idx;
	  }
  }

  return idx;

}



#endif

int Message::recvPkt(char *msg)
{
	int retVal; 
/* [21-04-2015]	[SG]	long vs int in 64 bit arch	*/
    struct x {
#ifdef _64BIT
	int pktSize;
#else
    long pktSize;
#endif
    } x;
#ifdef _64BIT
	int pktLength;
#else
	u_long pktLength;
#endif

    // get teh header size
	if ( (retVal = readData((char *)&x, SIZELEN)) != SIZELEN )
	{
         return retVal;
	}

#ifdef TESTING
	if ( x.pktSize > MAXPKTSZ )
	{
      sprintf(szError, " Size=%d", x.pktSize);
      	fprintf(stderr, "%s\n",szError);
	}
#endif
	// get the message

//	pktSizeBuff[SIZELEN] = '\0';
//	x = (struct x *)&pktSizeBuff;

	pktLength = ntohl((u_long) x.pktSize);

	if ( (retVal = readData(msg, pktLength)) != pktLength)
		return retVal;
	else
		return pktLength;
}



int Message::recvPkt(char *msg, int *type)
{
	GenMsgU *gm;
	int retVal; 
    struct x {
    long pktSize;
    } x;
	u_long pktLength;

    // get teh header size
	if ( (retVal = readData((char *)&x, SIZELEN)) != SIZELEN )
	{
         return retVal;
	}

	// get the message

//	pktSizeBuff[SIZELEN] = '\0';
//	x = (struct x *)&pktSizeBuff;

	pktLength = ntohl((u_long) x.pktSize);

	retVal = readData(msg, pktLength);
	
	gm = (GenMsgU *)msg;

	if ( retVal >= sizeof(gm->type) )
		*type = ntohl(gm->type);
	else
		*type = -1;

	return retVal;
}


struct MyMsg
{
	u_long sz;
	unsigned char msg[MAXPKTSZ];
};

int Message::sendPkt(char *msg, int len)
{
	MyMsg mMsg;

// Send a string from the server to the client.
//	u_long pktLength;

//	pktLength = htonl((u_long) len);

	mMsg.sz = htonl((u_long) len);
	memcpy((char *)mMsg.msg, msg, len);


  if (send (fd, (char *)&mMsg, len + sizeof(u_long), 0)
           == SOCKET_ERROR) 
  {
	
	return SOCKET_ERROR;
  }

  return 1;
}



int Message::sendPkt(char *msg, unsigned int len)
{
// Send a string from the server to the client.
	MyMsg mMsg;

//	u_long pktLength;

//	pktLength = htonl((u_long) len);
	mMsg.sz = htonl((u_long) len);
	memcpy((char *)mMsg.msg, msg, len);

  if (send (fd, (char *)&mMsg, len + sizeof(u_long), 0)
           == SOCKET_ERROR) 
  {
	return SOCKET_ERROR;
  }

  return 1;
}




int Message::sendPkt(char *buf, int type, int len )
{
	MyMsg mMsg;
	GenMsgU gm;
	u_long pktSz;
	int iVal;

	//printf("\n in send pkt");


// Send a string from the server to the client.

//	pktLength = htonl((u_long) len);

	gm.type = htonl((u_long)type);
	memcpy((char *)gm.body, buf, len);

	pktSz = len + sizeof(gm.type);

	mMsg.sz = htonl(pktSz);
	memcpy((char *)mMsg.msg, (char *)&gm, pktSz);

  if ((iVal = send (fd, (char *)&mMsg, (pktSz + sizeof(u_long)), 0))
           == SOCKET_ERROR) 
  {

	fprintf(stderr, "sending error ");
	return SOCKET_ERROR;
  }

  return iVal;

}


int Message::closeClient()
{
	return(close(fd));
}


int setRcvTimeOut(int fd, unsigned int timeout)
{

	return (setsockopt( 
        fd, 
        SOL_SOCKET, 
        SO_RCVTIMEO, 
        (char *)&timeout, 
        sizeof(timeout)));

}


/*
*
*/
int setKeepAlive(int fd, int v, int interval )
{
#if 0
	struct tcp_keepalive {
    u_long  onoff;
    u_long  keepalivetime;
    u_long  keepaliveinterval;
}tka;
 

	tka.onoff = 1;
	tka.keepalivetime = interval;
	tka.keepaliveinterval = interval;

	if ( WSAIoctl (fd, SIO_KEEPALIVE_VALS,
					&tka, sizeof(tka),
					NULL,0, NULL, NULL) != 0 )

		return SOCKET_ERROR;

	else
		return 0;
#else
	return (setsockopt( 
        fd, 
      SOL_SOCKET, 
        SO_KEEPALIVE, 
        (char *)&v, 
        sizeof(v)));
#endif
}


/*
*
*/
int Message::recvPktRaw(char *msg, int len)
{
	
	int retVal;

	retVal = readData(msg, len);
	

	return retVal;
}


/*
*/
int Message::sendPktRaw(char *msg, int len)
{
	char tBuff[MAXPKTSZ + MAXPKTSZ];


	memcpy(tBuff,msg, len);

  if (send (fd, tBuff, len , 0)
           == SOCKET_ERROR) 
  {

	return SOCKET_ERROR;
  }

  return 1;
}
