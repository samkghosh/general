/******************************************************************************\
* ping.c - Simple ping utility using SOCK_RAW
* 
*       This is a part of the Microsoft Source Code Samples.
*       Copyright 1996-1997 Microsoft Corporation.
*       All rights reserved.
*       This source code is only intended as a supplement to
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the
*       Microsoft samples programs.
\******************************************************************************/

#pragma pack(4)

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#include "easy.h"


#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0

#define ICMP_MIN 8 // minimum 8 byte icmp packet (just header)

/* The IP header */
typedef struct iphdr {
	unsigned int h_len:4;          // length of the header
	unsigned int version:4;        // Version of IP
	unsigned char tos;             // Type of service
	unsigned short total_len;      // total length of the packet
	unsigned short ident;          // unique identifier
	unsigned short frag_and_flags; // flags
	unsigned char  ttl; 
	unsigned char proto;           // protocol (TCP, UDP etc)
	unsigned short checksum;       // IP checksum

	unsigned int sourceIP;
	unsigned int destIP;

}IpHeader;

//
// ICMP header
//
typedef struct _ihdr {
  BYTE i_type;
  BYTE i_code; /* type sub code */
  USHORT i_cksum;
  USHORT i_id;
  USHORT i_seq;
  /* This is not the std header, but we reserve space for time */
  ULONG timestamp;
}IcmpHeader;

#define STATUS_FAILED 0xFFFF
#define DEF_PACKET_SIZE 32
#define MAX_PACKET 1024

#define xmalloc(s) HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(s))
#define xfree(p)   HeapFree (GetProcessHeap(),0,(p))

void fill_icmp_data(char *, int);
USHORT checksum(USHORT *, int);
void decode_resp(char *,int ,struct sockaddr_in *);

int egRemoteAlive(char *argv, int tmout)
{

  WSADATA wsaData;
  SOCKET sockRaw;
  struct sockaddr_in dest,from;
  struct hostent * hp;
  int bread,datasize;
  int fromlen = sizeof(from);
 //int timeout = 10;//1000;
  int timeout = tmout;
  char *dest_ip;
  char *icmp_data;
  char *recvbuf;
  unsigned int addr=0;
  USHORT seq_no = 0;

  if (WSAStartup(MAKEWORD(2,1),&wsaData) != 0)
  {
	  egerrno = _EG_SOCKSTARTFAIL;
	  return -1;
  }

#if 0
  sockRaw = WSASocket (AF_INET,
					   SOCK_RAW,
					   IPPROTO_ICMP,
					   NULL, 0,0);
#endif
   sockRaw = WSASocket (AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0,
                         WSA_FLAG_OVERLAPPED);
  if (sockRaw == INVALID_SOCKET) 
  {
	egerrno = WSAGetLastError();
	return -1;
  }
  bread = setsockopt(sockRaw,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,
  					sizeof(timeout));
  if(bread == SOCKET_ERROR) 
  {
	  closesocket(sockRaw);
	  egerrno = WSAGetLastError();
	  return -1;
  }
//  timeout = 1000;
  bread = setsockopt(sockRaw,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout,
  					sizeof(timeout));
  if(bread == SOCKET_ERROR) 
  {
  	  closesocket(sockRaw);

	  egerrno = WSAGetLastError();
	  return -1;
  }
  memset(&dest,0,sizeof(dest));

  if ( strchr(argv,'.') == NULL)
	  hp = gethostbyname(argv);
  else
  //if (!hp)
  {
	//int err= WSAGetLastError();
	hp = NULL;
	addr = inet_addr(argv);
  }  
  
  if ((!hp)  && (addr == INADDR_NONE) ) 
  {
  	  closesocket(sockRaw);

	  egerrno = _EG_SOCKSTARTFAIL;
	  return -1;
  }

  if (hp != NULL)
	  memcpy(&(dest.sin_addr),hp->h_addr,hp->h_length);
  else
  	dest.sin_addr.s_addr = addr;

  if (hp)
	  dest.sin_family = hp->h_addrtype;
  else
	  dest.sin_family = AF_INET;

  dest_ip = inet_ntoa(dest.sin_addr);

  datasize = DEF_PACKET_SIZE;
	
  datasize += sizeof(IcmpHeader);  

  icmp_data = (char *)xmalloc(MAX_PACKET);
  recvbuf = (char *)xmalloc(MAX_PACKET);

  if (!icmp_data) 
  {
	  xfree(recvbuf);
  	  closesocket(sockRaw);
	  WSACleanup();
	  egerrno = WSAGetLastError();
	  return -1;
  }
  

  memset(icmp_data,0,MAX_PACKET);
  fill_icmp_data(icmp_data,datasize);

  //while(1) 
  {
	int bwrote;
	
	((IcmpHeader*)icmp_data)->i_cksum = 0;
	((IcmpHeader*)icmp_data)->timestamp = GetTickCount();

	((IcmpHeader*)icmp_data)->i_seq = seq_no++;
	((IcmpHeader*)icmp_data)->i_cksum = checksum((USHORT*)icmp_data, 
											datasize);

	bwrote = sendto(sockRaw,icmp_data,datasize,0,(struct sockaddr*)&dest,
					sizeof(dest));
	if (bwrote == SOCKET_ERROR)
	{
	  if (WSAGetLastError() == WSAETIMEDOUT) 
	  {
		  egerrno = _EG_TIMEOUT;
	  }
	  else
		  egerrno = WSAGetLastError();
	 
	  xfree(icmp_data);
	  xfree(recvbuf);
	  closesocket(sockRaw);
	  WSACleanup();
	  return -1;
	}

	//if (bwrote < datasize ) {
	  //fprintf(stdout,"Wrote %d bytes\n",bwrote);
	//}
	
	bread = recvfrom(sockRaw,recvbuf,MAX_PACKET,0,(struct sockaddr*)&from,
	//bread = recvfrom(sockRaw,recvbuf,MAX_PACKET,MSG_PEEK,(struct sockaddr*)&from,
	
				 &fromlen);
	if (bread == SOCKET_ERROR)
	{
	  if (WSAGetLastError() == WSAETIMEDOUT) 
	  {
		  egerrno = _EG_TIMEOUT;
	  }
	  else
	  {
		  egerrno = WSAGetLastError();
	  }
  	  xfree(icmp_data);
	  xfree(recvbuf);

      closesocket(sockRaw);
	  WSACleanup();
      return -1;
	}

//	decode_resp(recvbuf,bread,&from);
  }
  xfree(icmp_data);
  xfree(recvbuf);
  closesocket(sockRaw);
  WSACleanup();
  return 0;

}
/* 
	The response is an IP packet. We must decode the IP header to locate 
	the ICMP data 
*/
void decode_resp(char *buf, int bytes,struct sockaddr_in *from) {

	IpHeader *iphdr;
	IcmpHeader *icmphdr;
	unsigned short iphdrlen;

	iphdr = (IpHeader *)buf;

	iphdrlen = iphdr->h_len * 4 ; // number of 32-bit words *4 = bytes

	if (bytes  < iphdrlen + ICMP_MIN) {
		printf("Too few bytes from %s\n",inet_ntoa(from->sin_addr));
	}

	icmphdr = (IcmpHeader*)(buf + iphdrlen);

	if (icmphdr->i_type != ICMP_ECHOREPLY) {
		fprintf(stderr,"non-echo type %d recvd\n",icmphdr->i_type);
		return;
	}
	if (icmphdr->i_id != (USHORT)GetCurrentProcessId()) {
		fprintf(stderr,"someone else's packet!\n");
		return ;
	}
	printf("%d bytes from %s:",bytes, inet_ntoa(from->sin_addr));
	printf(" icmp_seq = %d. ",icmphdr->i_seq);
	printf(" time: %d ms ",GetTickCount()-icmphdr->timestamp);
	printf("\n");
		
}


USHORT checksum(USHORT *buffer, int size) {

  unsigned long cksum=0;

  while(size >1) {
	cksum+=*buffer++;
	size -=sizeof(USHORT);
  }
  
  if(size ) {
	cksum += *(UCHAR*)buffer;
  }

  cksum = (cksum >> 16) + (cksum & 0xffff);
  cksum += (cksum >>16);
  return (USHORT)(~cksum);
}
/* 
	Helper function to fill in various stuff in our ICMP request.
*/
void fill_icmp_data(char * icmp_data, int datasize){

  IcmpHeader *icmp_hdr;
  char *datapart;

  icmp_hdr = (IcmpHeader*)icmp_data;

  icmp_hdr->i_type = ICMP_ECHO;
  icmp_hdr->i_code = 0;
  icmp_hdr->i_id = (USHORT)GetCurrentProcessId();
  icmp_hdr->i_cksum = 0;
  icmp_hdr->i_seq = 0;
  
  datapart = icmp_data + sizeof(IcmpHeader);
  //
  // Place some junk in the buffer.
  //
  memset(datapart,'E', datasize - sizeof(IcmpHeader));

}
