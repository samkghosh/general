/*
 * crc.c - routines for implementing CRC
 */
//#include "stdafx.h"
#include <stdio.h>
#include <memory.h>
#include "crc.h"


static unsigned char it[16] = {0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};
static unsigned short icrctb[256];
static unsigned char rchr[256];


 /* used by icrc to initialise its table */
unsigned short icrc1(unsigned short crc, unsigned char onech)
{
 int i;
 unsigned short ans=(crc^onech << 8);

 for (i = 0; i < 8; i++)
   {
     if (ans & 0x8000)
       ans = (ans <<= 1) ^ 4129;
     else
       ans <<= 1;
   }

 return ans;
}


void egInitcrc()
{
 static unsigned short init = 0;
 unsigned short j;


 if (!init)
   {
    init = 1;
    for (j = 0; j <= 255; j++)
      {
       icrctb[j] = icrc1(j << 8, (unsigned char) 0);
       rchr[j] = (unsigned char)(it[j & 0xF] << 4 | it[j >>4]);
      }
   }
}

unsigned short egIcrc(unsigned short crc, unsigned char *bufptr,
			     unsigned long len, short jinit, int jrev)
{

  unsigned short j, cword = crc;
  unsigned long jj;

  if(jinit >= 0)
    cword = ((unsigned char) jinit) | (((unsigned char) jinit) << 8);
  else if (jrev < 0)
    cword = rchr[HIBYTE(cword)] | rchr[LOBYTE(cword)] << 8;

#if 0
  for (j = 1; j <= len; j++)
    cword = icrctb[(jrev < 0 ? rchr[bufptr[j]] : bufptr[j]) ^ HIBYTE(cword)] ^ LOBYTE(cword) << 8;
#endif
   for (jj = 0; jj < len; jj++)
    cword = icrctb[(jrev < 0 ? rchr[bufptr[jj]] : bufptr[jj]) ^ HIBYTE(cword)] ^ LOBYTE(cword) << 8;

  return(jrev >= 0 ? cword : rchr[HIBYTE(cword)] | rchr[LOBYTE(cword)] << 8);
}


