/***********************************************************
* FileName: etheraddr.cpp
***********************************************************/ 
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>

/************************************* Global ****************/
#define MAXETHER	5

char *gEthers [32]={"eth","eth0", "eth1", "eth2", "eth3", "eth4"};

extern void logTrace(char *);

#define TESTOUT
char gszDiskIds[26][32];
int  gnDisks;
char gszMacAddr[50][32];
int  gnMacs;

extern int giVerify;
/************************************************************/

#if 0
int GetMACaddress()
{
	int s;
	struct ifreq buffer;
	int i;
        int err;
	
        gnMacs = 0;
	s = socket(PF_INET, SOCK_DGRAM, 0);


        for(i=0; i <MAXETHER; i++)
        { 
	   memset(&buffer, 0x00, sizeof(buffer));

	   strcpy(buffer.ifr_name, gEthers[i]);

	   err = ioctl(s, SIOCGIFHWADDR, &buffer);
        
	   if ( err == -1)
	   {
printf("Unable to find add for %s, errno=%d\n", gEthers[i], errno);
              continue;
	   }

	   sprintf(gszMacAddr[gnMacs++], "%.2X%.2X%.2X%.2X%.2X%.2X",
				 (unsigned char)buffer.ifr_hwaddr.sa_data[0],
				 (unsigned char)buffer.ifr_hwaddr.sa_data[1],
				 (unsigned char)buffer.ifr_hwaddr.sa_data[2],
				 (unsigned char)buffer.ifr_hwaddr.sa_data[3],
				 (unsigned char)buffer.ifr_hwaddr.sa_data[4],
				 (unsigned char)buffer.ifr_hwaddr.sa_data[5]
				);
#ifdef TESTOUT
	fprintf(stderr, "Addr=%s, MAC=%s\n", gEthers[i], gszMacAddr[gnMacs-1]);
#endif
        }
        close(s);	

	return 0;
}
#endif

int GetMACaddress()
{
	FILE *fp;
	char szBuf[128];
	char *pch;
	int i;

	fp = popen("ifconfig|grep HWaddr", "r");

	if ( fp == NULL)
		return 0;

	while(fgets(szBuf, sizeof(szBuf), fp) != NULL)
	{
		pch = strstr(szBuf,"HWaddr");
		if (pch == NULL)
			continue;
		pch = pch + 7;
		i =0;
		while( *pch != '\0' && *pch && '\r' && *pch != '\n' && *pch != ' ')
		{
			if (*pch != ':')
				gszMacAddr[gnMacs][i++] = *pch;	
			pch++;
		}
		gszMacAddr[gnMacs][i] = '\0';
		gnMacs++;
	}
	fclose(fp);
	return 0;
}
#ifdef USEMAIN
int main()
{
	int i;
	GetMACaddress();
	for(i=0; i < gnMacs; i++)
		printf("EAdd=%s\n", gszMacAddr[i]);

}
#endif
