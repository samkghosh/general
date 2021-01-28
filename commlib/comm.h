/*
* FILENAME: comm.h
*Purpose: Interface and other message formats for TCP/IP communication
*Date: 16-01-2001
* By: SKG
*/
#ifndef __COMM.H__
#define __COMM.H__
              
/* include file details */
       

#define MAXMSGSZ	(MAXPKTSZ - (2* sizeof(int)))
typedef struct 
{
	int pktSize; // total size except this field size
	int pktType; // type of the packet
	unsigned char body[MAXMSGSZ];
}GenMsg;


#define CLIENT	1
#define SERVER	2

Class PSocketTCP:public CSocket
{
   
   public:
	   PSocket(int type, char * IPAddtress=NULL, int Port = 0);
	   ~PSocket();
       int send(int fd, unsigned char *buff, int mode);
	   int receive(int fd, unsigned char *buff, int type, int size); 
   
#endif                                  