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


void initcrc()
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

unsigned short icrc(unsigned short crc, unsigned char *bufptr,
			     unsigned long len, short jinit, int jrev)
{

  unsigned short j, cword = crc;


  if(jinit >= 0)
    cword = ((unsigned char) jinit) | (((unsigned char) jinit) << 8);
  else if (jrev < 0)
    cword = rchr[HIBYTE(cword)] | rchr[LOBYTE(cword)] << 8;

#if 0
  for (j = 1; j <= len; j++)
    cword = icrctb[(jrev < 0 ? rchr[bufptr[j]] : bufptr[j]) ^ HIBYTE(cword)] ^ LOBYTE(cword) << 8;
#endif
   for (j = 0; j < len; j++)
    cword = icrctb[(jrev < 0 ? rchr[bufptr[j]] : bufptr[j]) ^ HIBYTE(cword)] ^ LOBYTE(cword) << 8;

  return(jrev >= 0 ? cword : rchr[HIBYTE(cword)] | rchr[LOBYTE(cword)] << 8);
}


#if 0
int main(void)
{
 unsigned char lin[1024];
 unsigned short i1, i2;
 unsigned long n;
 FILE *fp;
 unsigned char ch;

 initcrc();

#if 0
     printf("Enter length and string: \n");
     if (scanf("%lu %s", &n, &lin[1]) == EOF)
       return;

     lin[n+1] = '\0';
     printf("%s\n", &lin[1]);
#endif

#if 1
     n = 512;

     memset(lin, '\0',sizeof(lin));
     fp = fopen("x","r+b");
     fread(lin,512, 1, fp);
     fclose(fp);
#endif
#if 0
     i1 = icrc(0, lin, 512, (short)0, 1);
     lin[n+1] = HIBYTE(i1);
     lin[n+2] = LOBYTE(i1);
     i2 = icrc(i1, lin, n+2, (short)0, 1);
     printf("   XMODEM: String CRC, Packet CRC = 	0x%x	0x%x\n", i1, i2);

     i1 = icrc(i2, lin, n, (short)0xff, -1);
     lin[n+1] = ~LOBYTE(i1);
     lin[n+2] = ~HIBYTE(i1);
     i2 = icrc(i1, lin, n+2, (short)0xff, -1);
     printf("     X.25: String CRC, Packet CRC = 	0x%x	0x%x\n", i1, i2);
#endif

 //    i1 = icrc(i2, lin, n, (short)0, -1);
     i1 = icrc(0, lin, n, (short)0, -1);
     lin[n+1] = LOBYTE(i1);
     lin[n+2] = HIBYTE(i1);
     //i2 = icrc(i1, lin, n+2, (short)0, -1);
     printf("CRC-CCITT: String CRC, Packet CRC = 	0x%x	0x%x\n", i1, i2);

     ch = lin[1];
     printf("Introducing error\n");
     lin[1] = '#';

#if 0
     i1 = icrc(0, lin, n, (short)0, 1);
     lin[n+1] = HIBYTE(i1);
     lin[n+2] = LOBYTE(i1);
     i2 = icrc(i1, lin, n+2, (short)0, 1);
     printf("   XMODEM: String CRC, Packet CRC = 	0x%x	0x%x\n", i1, i2);
     i1 = icrc(i2, lin, n, (short)0xff, -1);
     lin[n+1] = ~LOBYTE(i1);
     lin[n+2] = ~HIBYTE(i1);
     i2 = icrc(i1, lin, n+2, (short)0xff, -1);
     printf("     X.25: String CRC, Packet CRC = 	0x%x	0x%x\n", i1, i2);
#endif

     i1 = icrc(0, lin, n, (short)0, -1);
     lin[n+1] = LOBYTE(i1);
     lin[n+2] = HIBYTE(i1);
     //i2 = icrc(i1, lin, n+2, (short)0, -1);
     printf("CRC-CCITT: String CRC, Packet CRC = 	0x%x	0x%x\n", i1, i2);

     printf("Removing error\n");
     lin[1] = ch;

     i1 = icrc(0, lin, n, (short)0, -1);
     lin[n+1] = LOBYTE(i1);
     lin[n+2] = HIBYTE(i1);
     //i2 = icrc(i1, lin, n+2, (short)0, -1);
     printf("CRC-CCITT: String CRC, Packet CRC = 	0x%x	0x%x\n", i1, i2);

 printf("Normal completion\n");
 return(0);
}
#endif

#if 0

int main(int argc, char *argv[])
{
 unsigned char lin[1024];
 unsigned short i1, i2, CRC;
 unsigned long n;
 FILE *fp;
 unsigned char ch;
 int i;

 initcrc();

  if ( argc != 2 )
  {
     exit(1);
  }

  if ( (n = atoi(argv[1])) > 0 )
  {
     for ( i = 0; i < n; i++ )
	lin[i] = i % 255;

     CRC = icrc(0, lin,
		  n, (short)0, -1);
     lin[n] = LOBYTE(CRC);
     lin[n+1] = HIBYTE(CRC);
     if ( (fp = fopen("x", "w+b")) == NULL )
    {
	printf("Error in open x to write\n");
	exit(1);
    }

     fwrite(lin, n+2, 1, fp);
     fclose(fp);
  }
  else
  {
    if ( (fp = fopen("x", "r+b")) == NULL )
    {
	printf("Error in open x\n");
	exit(1);
    }

    n = fread(lin,1, 1024, fp);

    n=n-2;

    fclose(fp);

    CRC = icrc(0, lin, n, (short)0, -1);

//      printf("CRC=%x:%x Computed CRC=%x:%x, Length=%d\n",
 //			lin[n], lin[n+1], LOBYTE(CRC), HIBYTE(CRC), n+1);
    if((lin[n] != LOBYTE(CRC)) || lin[n+1] != HIBYTE(CRC))
      {
	     printf("CRC=%x:%x Computed CRC=%x:%x, Length=%d\n",
			lin[n], lin[n+1], LOBYTE(CRC), HIBYTE(CRC), n-2);
      }


  }
}
#endif