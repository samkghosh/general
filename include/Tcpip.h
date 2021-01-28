// Tcpip.h: interface for the Tcpip class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __TCPIP_H__
#define __TCPIP_H__

#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;



#define MAX_PENDING_CONNECTS  4       // Maximum length of the queue 
                                      // of pending connections
#define FOREVER  for(;;)

#define SOCKET_TIMEOUT	-4
#define INVALIDOBJECT	-3
#define SOCKET_CLOSED	-2
#define SOCKERR		-1
#define OK			1
#define INVALID_SOCKET -1

#define SIZELEN    (sizeof(long))
#define MAXPKTSZ	(1024 * 8)
#define TRUE 1
#define FALSE 0
#define SOCKET_ERROR -1

class Tcpip
{
private:
	int  winSocket;
	int  clientSocket;
     struct sockaddr_in  local_sin,              // Local socket address
              accept_sin;             // Receives the address of the 
                                      // connecting entity
    bool clientOrServer; // client or server TRUE: Server else Client
   
	
    
    char szError[128];
#if 0
	char buff[MAXPKTSZ];
	char *currPtr;

	int readData(char *, int);
#endif	
    void initServer(int);
    void initClient(int, char *);

public:
	 bool errorFlag;
 
	Tcpip();
	Tcpip(int);
	Tcpip(int, char *);
	virtual ~Tcpip();
    int waitForConnection();
#if 0
	int sendPkt(char *, int);
	int recvPkt(char *);
#endif

	int listenClient(int port);
	int connectServer(int port, char *host);
        void closeClient();
	inline int  getFd();
	
};

inline int Tcpip::getFd()
{
	return clientSocket;
}



#endif
