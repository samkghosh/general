/****************************************************************
*
* FileName:utils.cpp
*
* Process: Message sending utility
*
* Purpose:
*
* Notes:
** NONE *
*
* Global routines:
*
* Local routines: 
* * NONE *
*
* Revision History:
* 
*Tag No. Date AuthorDescription
*---------- -----------------
*
**************************************************************/
#include <stdio.h>
#include <string.h>
#include "Message.h"

int sndMsgTo(char *ipAddr, int portNum, void * msg, int len, int type)
{
	Tcpip tcpc(portNum, ipAddr);
	int fd;
	Message *outMsg;
	char errMsg[512];
	int rc;

	if ( tcpc.errorFlag == true )
	{
		sprintf(errMsg,"Couldnot Create Send object for IP=%s, port=%d", ipAddr, portNum);
//		McDbgLog(LOGLEVEL0, errMsg);

	printf("%s\n",errMsg);
		return SOCKET_ERROR;
	}

	fd = tcpc.getFd();

	outMsg = new Message(fd);

	rc = outMsg->sendPkt((char *)msg, type, len);

	//SleepEx(250, true);
       // sleep(250);
	//printf("\n deleting message");

	delete outMsg;

	return rc;
}
