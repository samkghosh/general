// Tcpip.cpp: implementation of the Tcpip class.
//
//////////////////////////////////////////////////////////////////////
#include "Tcpip.h"
#include <string.h>
#include <memory.h>
#include <stdio.h>


#undef TESTING
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////
Tcpip::~Tcpip()
{


  // Disable both sending and receiving on ClientSock.
  //shutdown (clientSocket, 0x02);

  // Close ClientSock.
  close(clientSocket);

  close(winSocket);

  
}


void Tcpip::closeClient()
{
	
   // Disable both sending and receiving on ClientSock.
   //shutdown (clientSocket, 0x02);

   close(clientSocket);
}




Tcpip::Tcpip()
{
   errorFlag = FALSE;
}


Tcpip::Tcpip(int port)
{
   initServer(port);
}



Tcpip::Tcpip(int port, char *host)
{
   initClient(port, host);
}



void Tcpip::initServer(int port)
{
	
	winSocket = INVALID_SOCKET,  // Window socket
    clientSocket = INVALID_SOCKET; // Socket for communicating 
                                      // between the server and client
 
  // Create a TCP/IP socket, WinSocket.
  if ((winSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
  {
#ifdef TESTING
        perror("socket()");
#endif

	errorFlag = TRUE;
    return;
  }
    // Fill out the local socket's address information.
  local_sin.sin_family = AF_INET;
  local_sin.sin_port = htons (port);  
  local_sin.sin_addr.s_addr = htonl (INADDR_ANY);

  // Associate the local address with WinSocket.
  if (bind (winSocket, 
            (struct sockaddr *) &local_sin, 
            sizeof (local_sin)) == SOCKET_ERROR) 
  {
#ifdef TESTING
      perror("bind()");
#endif

    close(winSocket);
	errorFlag = TRUE;
    return ;
  }


    // Establish a socket to listen for incoming connections.
  if (listen (winSocket, MAX_PENDING_CONNECTS) == SOCKET_ERROR) 
  {
#ifdef TESTING
     perror("listen()");
#endif

    close (winSocket);
	errorFlag = TRUE;
    return;
  }
   
  int optVal = 1;
  // set socket option
	if ( setsockopt( winSocket,SOL_SOCKET,	SO_KEEPALIVE,              
					(char *)&optVal,
					sizeof(int)) != 0)
	{
	    close(winSocket);
		errorFlag = TRUE;
		return;
	}

	optVal = 2 * MAXPKTSZ;

	setsockopt( 
        winSocket, 
        SOL_SOCKET, 
        SO_RCVBUF, 
		(char  *)&optVal,
        sizeof(int));

	setsockopt( 
        winSocket, 
        SOL_SOCKET, 
        SO_SNDBUF, 
		(char  *)&optVal,
        sizeof(int));

  clientOrServer = TRUE;
  errorFlag = FALSE;
}


void Tcpip::initClient(int port, char * hostName)
{
    struct hostent * phostent = NULL;           // Points to the HOSTENT structure
                                      // of the server
    int retry = 1;
	char *dotPtr;
	
	clientOrServer = FALSE;

#if 0
    totalCount = 0;
	currPtr = buff;
#endif

    
    winSocket = INVALID_SOCKET,  // Window socket
    clientSocket = INVALID_SOCKET; // Socket for communicating 
                                      // between the server and client
   
	// Create a TCP/IP socket that is bound to the server.
  if ((clientSocket = socket (AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
  {
#ifdef TESTING

          perror("socket()");
     
#endif

	errorFlag = TRUE;
    return;
  }

  // Fill out the server socket's address information.
  local_sin.sin_family = AF_INET;

   if ( (dotPtr = strchr(hostName, '.')) == NULL )
   {

     // Retrieve the host information corresponding to the host name.
     if ((phostent = gethostbyname (hostName)) == NULL) 
	 {
#ifdef TESTING
		 perror("gethostbyname()");
#endif

       close (clientSocket);
	   errorFlag = TRUE;
       return;
	 }

     // Assign the socket IP address.
     memcpy ((char *)&(local_sin.sin_addr), 
          phostent->h_addr, 
          phostent->h_length);
   }
   else
   {  u_long x;

       x = inet_addr(hostName);
       //local_sin.sin_addr = htonl(inet_addr(hostName));
	   memcpy((char *)&(local_sin.sin_addr), &x,
	                  sizeof(unsigned long));
   }


  // Convert to network ordering.
  local_sin.sin_port = htons (port);      

 errorFlag = FALSE;

  while (retry != 0 )
  {
		// Establish a connection to the server socket.
		if (connect (clientSocket, 
               (struct sockaddr*) &local_sin, 
               sizeof (local_sin)) == SOCKET_ERROR) 
		{
#ifdef TESTING
			perror("connect()");
#endif
			
//	    close(clientSocket);
			errorFlag = TRUE;
		 // SleepEx(1000, true);
		  retry --;
		  continue;
//		  return;
		}
		else
		{
			errorFlag = FALSE;
			return;
		}
  }

  if ( retry == 0 )
  {
	  errorFlag = TRUE;
	  return;
  }
// set socket option
  int optVal = 1;
	if ( setsockopt(
					 clientSocket,                 
					SOL_SOCKET,                
					SO_KEEPALIVE,              
					(char *)&optVal,
					sizeof(int)) != 0)
	{
	    close(winSocket);
		errorFlag = TRUE;
		return;
	}


  //clientOrServer = FALSE;
  errorFlag = FALSE;
  return;
}




int Tcpip::waitForConnection()
{
  int accept_sin_len;                 // Length of accept_sin


  errorFlag = FALSE;

  if ( clientOrServer == FALSE)
	  return INVALIDOBJECT;
#if 0
  totalCount = 0;
  currPtr = buff;
#endif

  accept_sin_len =sizeof (accept_sin);

  // Accept an incoming connection attempt on WinSocket.
  clientSocket = accept (winSocket, 
                       (struct sockaddr *)&accept_sin, 
                        (socklen_t*)&accept_sin_len);

  if (clientSocket == INVALID_SOCKET) 
  {
#ifdef TESTING

    perror("accept()");
#endif

    errorFlag = TRUE;
    return SOCKET_ERROR;
  }
 int optVal = 1;
  // set socket option
	if ( setsockopt(
					 winSocket,                 
					SOL_SOCKET,                
					SO_KEEPALIVE,              
					(char *)&optVal,
					sizeof(int)) != 0)
	{
	    close(clientSocket);
		errorFlag = TRUE;
		return SOCKET_ERROR;
	}
  return TRUE;
}



int Tcpip::listenClient(int port)
{
	initServer(port);

   if (errorFlag == TRUE )
	   return SOCKET_ERROR;
   else
	   return OK;
}


int Tcpip::connectServer(int port, char * host)
{
   initClient(port, host);
   if (errorFlag == TRUE )
	   return SOCKET_ERROR;
   else
	   return OK;
}





