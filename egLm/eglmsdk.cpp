/***************************************************************
** eglmsdk.cpp
**
** Purpose:
*
** Global Procedures Defined:
**
**
** Global Procedures Referenced:
**
**
** Global Data Defined:
**
**
** Global Deta Referenced:
**
**
** Revision History:
**
** [0]	11-07-2002	SG	Created
**
****************************************************************/
#ifdef WIN32
#include "stdAfx.h"
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef WIN32
#include <io.h>
#include <direct.h>
#include <winuser.h>
#endif
#define stricmp strcasecmp
#define egStrip Strip
#define egStrtok Strtok
#define egConvertTime ConvertTime

#include "eglmsdkl.h"

#include "eglmsdk.h"
#include "egLm.h"

#ifdef WIN32
#include "easy.h"
#include "cRegProcess.h"
#endif

#include "crc.h"

#define EGLMFILENAME	"EGLM_LicenseFile"
int getShCtiBS(char *szSln);


int EgLMTotalLicenseCount;

T_LMData LMData[MAXLICENSE];
char EgLMServerIpAddr[16];

extern char gszDiskIds[26][32];
extern int  gnDisks;
extern char gszMacAddr[50][32];
extern int  gnMacs;

char		gszTraceFileName[256];

void logTrace(char *msg);

/* [25012006]	[SG] */
int giVerify;

char * Strip(char *x, char chr =' ');
char * Strtok( char * pCh, char *token, char * sep);
unsigned int ConvertTime(int dd, int mm, int yy, int hh, int min, int ss);
/****************************************************************
** Function Name: egLMgetLMDataFlName
** Purpose: To get the License manager's Filename 
**
** Algorithm:
**
** 
******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"getLMDataFlName"
int egLMgetLMDataFlName(char *flName)
{
	char szErrMsg[256];

	flName[0] = '\0';
#ifdef WIN32
	char * pCh;
	int rc;
	char szWinDir[MAX_PATH+1] = "";
	FILE *fp;

	pCh = getcwd(NULL,0);

	if ( pCh != NULL)
	{
		sprintf(flName,"%s\\license.dat",pCh);
		free(pCh);
	}
	else
		strcpy(flName,"license.dat");


	if ( (fp = fopen(flName,"r+t")) == NULL)
	{
///			sprintf(szErrMsg,"License file =%s Not Found[Errorcode=%d]", flName, errno);
//			MessageBoxA(NULL, szErrMsg, "License Error", MB_OK);

	}
	else
	{
		fclose(fp);
	}
	//		strcpy(gszTraceFileName,"Eglm_Trace.log");

	strcpy(gszTraceFileName,"Eglm_Trace.log");

	pCh = getenv(EGLMFILENAME);

	if ( pCh != NULL)
	{
		strcpy(flName, pCh);
		strcpy(gszTraceFileName, flName);

		pCh = strrchr(gszTraceFileName,'\\');

		if ( pCh == NULL)
		{
			strcpy(gszTraceFileName,"Eglm_Trace.log");
		}
		else
		{
			pCh++;
			*pCh = '\0';

			strcat(gszTraceFileName,"Eglm_Trace.log");
		}
			
		if ( access(flName, 0 ) == -1 )
		{
			sprintf(szErrMsg,"License file =%s Not Found", flName);
			MessageBoxA(NULL, szErrMsg, "License Error", MB_OK);
//			AfxMessageBox(szErrMsg, MB_OK);
	
		}
	}
	else
	{
		if ( access(flName, 0 ) == -1 )
		{
	//		sprintf(szErrMsg,"Access Error [license.dat] Code=%d", errno);
	//		MessageBoxA(NULL, szErrMsg, "License Error", MB_OK);

			rc = GetWindowsDirectory(szWinDir, MAX_PATH);

			if ( rc != 0 )
			{
				szWinDir[rc] = '\0';
					
				if (szWinDir[rc-1] == '\\')
					szWinDir[rc-1] = '\0';
				sprintf(flName, "%s\\eglm\\license.dat", szWinDir);

				sprintf(gszTraceFileName, "%s\\eglm\\Eglm_Trace.log", szWinDir);

				if ( access(flName, 0 ) == -1 )
				{
				//	sprintf(szErrMsg,"License file =%s Not Found", flName);
				//	MessageBoxA(NULL, szErrMsg, "License Error", MB_OK);
				//	AfxMessageBox(szErrMsg, MB_OK);
				
				}


			}
		}
		else
		{
			//strcpy(flName,"license.dat");
			strcpy(gszTraceFileName,"Eglm_Trace.log");

			if ( access(flName, 0 ) == -1 )
			{
				sprintf(szErrMsg,"License file =%s Not Found", flName);
				MessageBoxA(NULL, szErrMsg, "License Error", MB_OK);
			//	AfxMessageBox(szErrMsg, MB_OK);
			
			}

		}

	}

	if ( giVerify == 1)
	{
		sprintf(szErrMsg,"License File Found: [%s]", flName);
		logTrace(szErrMsg);
		printf("\n%s\n", szErrMsg);
	}
#else
	strcpy(flName, "/etc/egLm/license.dat");
	sprintf(gszTraceFileName, "/tmp/Eglm_Trace.log");
#endif
	if ( flName[0] == '\0')
		return 0;
	else
	{
		sprintf(szErrMsg,"************* EGLM Start ************************");
		logTrace(szErrMsg);

		return 1;
	}
}





/****************************************************************
** Function Name: egLMCheck
** Purpose: To get the integrity of the license data file
**
** Algorithm:
**
**			return EGLM_INVALID;
** 			return EGLM_NOENTRYFOUND;
			return EGLM_TIMEEXPIRED;
 
******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"egLMCheck"
int egLMCheck(char * productId)
{
	int i;
	char *pch;
	int k;
	char szBuf[256]="";
	char szErrMsg[512];


	sprintf(szErrMsg,">>Request to verify License for [%s][%d]", productId, EgLMTotalLicenseCount);
	logTrace(szErrMsg);

//	MessageBox(NULL,szErrMsg,"Msg", MB_OK);

	for ( i = 0; i < EgLMTotalLicenseCount; i++)
	{
		sprintf(szErrMsg,">>[%d][%s] <<[%s]",i, LMData[i].productId, productId);
		logTrace(szErrMsg);

//		MessageBox(NULL,szErrMsg,"Msg", MB_OK);

		if ( stricmp(LMData[i].productId, productId) == 0)
			break;
	}

	if ( i == EgLMTotalLicenseCount)
		return EGLM_NOENTRYFOUND;

	// check the date
	if ( LMData[i].expDate != 0 )
	{
		if ( LMData[i].expDate < (unsigned long)time(NULL))
			return EGLM_TIMEEXPIRED;
	}
	// check the host id
	if ( LMData[i].hostIdType == HID_NONE )
	{
			return EGLM_OK;
	}

	if ( LMData[i].hostIdType == HID_MACID )
	{
		GetMACaddress();

		for ( k = 0; k < gnMacs; k++)
		{
			sprintf(szErrMsg,"Licence HostId=%s, M/c HostId=%s", LMData[i].hostId, gszMacAddr[k]);
			logTrace(szErrMsg);
			if (stricmp(LMData[i].hostId, gszMacAddr[k]) == 0 )
			{
				return EGLM_OK;
			}
		}

	}

	if ( LMData[i].hostIdType == HID_DISKID )
	{

		//getDiskSerialNum(NULL);

		for (k=0; k < gnDisks; k++)
		{
			sprintf(szErrMsg,"Licence HostId=%s, M/c HostId=[%s]", LMData[i].hostId, gszDiskIds[k]);
			logTrace(szErrMsg);

			if ( stricmp(gszDiskIds[k], LMData[i].hostId) == 0 )
			{
				logTrace("License Found OK");

				return EGLM_OK;
			}
		}

	}


	if ( LMData[i].hostIdType == HID_DIALOGIC)
	{
		szBuf[0] = '\0';

		if ( getDialogicBS(szBuf) == 0 )
		{
			pch = strtok(szBuf," ;,");
			
			while(pch != NULL)
			{
				if ( stricmp(pch, LMData[i].hostId) == 0 )
					return EGLM_OK;

				pch = strtok(NULL, " ;,");

			}
		}

	}

	if ( LMData[i].hostIdType == HID_ShCti)
	{
		szBuf[0] = '\0';
	

		if ( getShCtiBS(szBuf) == 0 )
		{

			sprintf(szErrMsg ,"Licence HostId=%s, ShCti DSL No=[%s]", LMData[i].hostId, szBuf);
			logTrace(szErrMsg);


			pch = strtok(szBuf," ;,");
			
			while(pch != NULL)
			{
				if ( stricmp(pch, LMData[i].hostId) == 0 )
					return EGLM_OK;

				pch = strtok(NULL, " ;,");

			}
		}

	}

	if ( LMData[i].hostIdType == HID_NMS)
	{
		sprintf(szErrMsg,"Licence Based On NMS Board. HostId=%s", LMData[i].hostId);
		logTrace(szErrMsg);

		szBuf[0] = '\0';

		if ( getNMSBS(szBuf) == 0 )
		{
			pch = strtok(szBuf," ;,");
			
			while(pch != NULL)
			{
				if ( stricmp(pch, LMData[i].hostId) == 0 )
					return EGLM_OK;

				pch = strtok(NULL, " ;,");

			}
		}

	}
	
	
	if ( LMData[i].hostIdType == 0 )
	{
		logTrace("No Host id found and key is passed OK");

		return EGLM_OK;
	}

	return EGLM_INVALID;
}




#if 1
/****************************************************************
** Function Name: egLMgetProductEntry
** Purpose: To get the Product entry from the license file
**		     Returns a null terminated string
**
** Algorithm:
**		return EGLM_OPENERROR; 

** 
******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"egLMgetProductEntry"
int egLMgetProductEntry(char * lmFileName, char * productId, char * buf)
{
	FILE *fp;
	char rBuf[1024];
	char * pCh;
	char token[128];
	bool lineFound;
	int lastIndex;


	buf[0] = '\0';
	// open the file
	if ( (fp = fopen(lmFileName, "r+t")) == NULL)
	{
		return EGLM_OPENERROR; 
	}

	// search for the product Id
	pCh = fgets(rBuf, sizeof(rBuf) -1, fp);

	lineFound = false;
	while (lineFound == false)
	{		
#ifdef TESTOUT 
fprintf(stderr, " Buffer Read=[%s]\n",rBuf); 
#endif
		egStrip(rBuf);
		pCh = rBuf;
		// check if there is any keyword "EGLM"
		pCh = egStrtok(pCh, token, " \\");

	
		if ( pCh != NULL )
		{
			if ( stricmp(token, "EGLM") == 0 )
			{
				pCh = egStrtok(pCh, token, " \t\\\r\n");
				if ( pCh != NULL)
				{
					if ( stricmp(token, productId) == 0 )
					{
						// product Id is found
						while(true)
						{
							lastIndex = strlen(rBuf);

							if ( rBuf[lastIndex -1] == '\r' || rBuf[lastIndex-1] == '\n')
							{
								rBuf[lastIndex -1] = '\0';
								lastIndex--;
							}

							if ( rBuf[lastIndex -1] == '\\')
							{
								rBuf[lastIndex - 1] = ' ';
								strcat(buf, rBuf);
							}
							else
							{
								strcat(buf, rBuf);
								lineFound = true;			
								break;
							}

							pCh = fgets(rBuf,sizeof(rBuf) -1, fp);

							if ( pCh == NULL)
							{
								lineFound = true;			
								break;
							}
							egStrip(rBuf);
						}//while
					} //if product is found
				}
				
			}// if License entry is found
		}

		if ( lineFound == true )
			break;

		pCh = fgets(rBuf, sizeof(rBuf) -1, fp);

		if ( pCh == NULL)
			if ( feof(fp))
				break;
	}
	// close file
	fclose(fp);

	if ( buf[0] == '\0')
		return 0;
	else
		return 1;
}





#endif


/****************************************************************
** Function Name: egGetKey
** Purpose: Create the Key
** Algorithm:
**
** 
******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"egGetKey"
int egGetKey(char *buf, char * key)
{

	unsigned short CRC;
	unsigned char keyBuf[1024];
	char *pCh = buf;
	char token[128];
	unsigned char hByte, lByte;
	time_t cTime;
//	unsigned int hhByte, llByte;
	unsigned long tTime, fTime;

	keyBuf[0] = '\0';
	while (pCh != NULL)
	{
		pCh = egStrtok(pCh, token, " ");
		
		if (strncmp(token,"KEY=",4) != 0 )
			strcat((char *)keyBuf, token);
//		else
//		{
//			strcpy(keyVal, token+4);
//		}
	}
#if 1
	initcrc();
	CRC = icrc(0, keyBuf, strlen((char *)keyBuf), (short)0, -1);
#endif
//	CRC = 12345;

	hByte = HIBYTE(CRC);
	lByte = LOBYTE(CRC);

	cTime = time(NULL);

	cTime = cTime &0x0000FFFF;

	tTime = cTime >> 12;
	tTime = tTime << 8;
	fTime = tTime | hByte;
	fTime = fTime << 8;
	tTime = (cTime >> 8) & 0x000000F0;
	fTime = fTime | tTime;
	fTime = fTime << 4;
	fTime = fTime | lByte;
	fTime = fTime << 8;
	fTime = fTime | (cTime & 0x000000FF);


	sprintf(key, "%u", fTime);

#if 0
	unsigned long cfTime = atol(keyVal);

	hhByte = cfTime & 0x0FF00000;
	llByte = cfTime & 0x0000FF00;

	hhByte = hhByte >> 20;
	llByte = llByte >> 8;
#endif
	return 1;
}





/****************************************************************
** Function Name: egLMVerifyKey
** Purpose: Verifies the Key
** Algorithm:
**
** 
******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"egLMVerifyKey"
bool egLMVerifyKey(char *buf)
{
	char	szMsg[512];
	unsigned short CRC;
	unsigned char keyBuf[1024];
	char *pCh = buf;
	char token[128];
	unsigned char hByte, lByte;
//	time_t cTime;
//	unsigned int thByte, tlByte; // temporary storage for hi/lo comp
	unsigned char vhByte, vlByte; // hi/lo of stored value
	unsigned long cfTime;
//	unsigned long tTime, fTime;
	char keyVal[512];

	sprintf(szMsg,"The Key To be Verified: %s", buf);

	logTrace(szMsg);

	if (giVerify)
		printf("\n%s\n", szMsg);

	keyBuf[0] = '\0';
	keyVal[0] = '\0';

	while (pCh != NULL)
	{
		pCh = egStrtok(pCh, token, " ");
		
		if (strncmp(token,"KEY=",4) != 0 )
			strcat((char *)keyBuf, token);
		else
		{
			strcpy(keyVal, token+4);
		}
	}
#if 1
	initcrc();
	CRC = icrc(0, keyBuf, strlen((char *)keyBuf), (short)0, -1);
#endif

	hByte = HIBYTE(CRC);
	lByte = LOBYTE(CRC);

	cfTime = 0L;
	
	int iLen, i;
	iLen = strlen(keyVal);
	
	for (i = 0; i < iLen; i++)
	{
		cfTime = (cfTime * 10) + ( keyVal[i] - '0' );
	}


	vhByte = (unsigned char)((cfTime & 0x0FF00000) >>20);
	vlByte = (unsigned char)((cfTime & 0x0000FF00) >> 8);


	/*
	printf("Value =%ud\n", cfTime);
	
	cfTime = atol(keyVal);
//printf("Value =%ld\n", cfTime);

		
	thByte = cfTime & 0x0FF00000;
	tlByte = cfTime & 0x0000FF00;

	vhByte = thByte >> 20;
	vlByte = tlByte >> 8;
*/
//double x = 	atol(keyVal);

/*	if ( giVerify)
	{
		if (hByte == vhByte) 
		{
			printf("Hi Byte matches\n");	
		}

		if (lByte == vlByte)
		{
			printf("Low Byte matches\n");	

		}
	}

	vhByte = 0x5b;
	vlByte = 0xc9;
*/

	if ( (hByte == vhByte) && (lByte == vlByte))
	{
		logTrace("The key validation Passed");
	//	printf("\nThe Key Validation Passed");
		return true;
	}
	else
	{
		sprintf(szMsg,"The key validation FAILED. Key is [%s][%x:%x::%x,%x]", keyVal,
																	hByte, vhByte, lByte, vlByte);

	//	printf("\n%s\n", szMsg);
		logTrace(szMsg);

		return false;
	}

	return false;
}




/*
*
*/
/****************************************************************
** Function Name: egInitLm
** Purpose: 
** Algorithm:
**
** 
******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"egInitLm"
int egInitLm(char * lmFileName)
{
	FILE *fp;
	char rBuf[1024]="";
	char * pCh=NULL;
	char token[128]="";
//	bool lineFound;
//	int lastIndex;
	int dd,mm,yyyy,hh,min,ss;
	int i;
	char szBuf[1024]="";
	char expDate[64]="";
	char	szErrMsg[1024]="";



	static int initialised;

	if ( initialised)
	{
#ifdef TESTOUT
		fprintf(stderr, "already initialised");
#endif
		return 1;
	}
	else
	{
#ifdef TESTOUT
            fprintf(stderr, "Initalising, LicenseFile=%s\n", lmFileName);
#endif

		initialised = 1;
	}
	EgLMTotalLicenseCount = 0;

	// open the file
	if ( (fp = fopen(lmFileName, "r+t")) == NULL)
	{
	    sprintf(szErrMsg,"Unable to open file=%s, Error=%d\n", lmFileName, errno);
	    logTrace(szErrMsg);
#ifdef TESTOUT
            fprintf(stderr, "%s", szErrMsg);
#endif
		return EGLM_OPENERROR; 
	}

#ifdef TESTOUT
            fprintf(stderr, "LicenseFile=%s Opened Successfully\n", lmFileName);
#endif

	while (true)
	{		
		if ( egLMGetValidEntry(fp, rBuf) == 0 )
			break;

#ifdef TESTOUT
            fprintf(stderr, "Valid Buf=[%s]\n", rBuf);
#endif
		egStrip(rBuf);

		// check if the entry is a correct and authentic entry
		if ( egLMVerifyKey(rBuf) == false)
		{
			sprintf(szErrMsg,"NOT A VALID ENTRY: %s", rBuf);
			logTrace(szErrMsg);

			continue;
		}

		pCh = rBuf;

		if (pCh != NULL)
		{
			EgLMTotalLicenseCount++;

			sprintf(szErrMsg,"VALID ENTRY: %s", rBuf);
			logTrace(szErrMsg);

			
			// skip the first token
			pCh = egStrtok(pCh, token, " \t\r\n");
			
			pCh = egStrtok(pCh, token, " \t\r\n");
			
			// check if the same product is already in the list
			for ( i = 0; i < EgLMTotalLicenseCount; i++)
			{
				if ( stricmp(LMData[EgLMTotalLicenseCount].productId, token) == 0 )
					break;
			}

			if ( pCh != NULL)
			{
				// copy the product Id
				strcpy(LMData[EgLMTotalLicenseCount-1].productId, token);

				if ( giVerify)
				{
					sprintf(szErrMsg,"Product Id: %s", token);
					logTrace(szErrMsg);
					printf("\n%s\n", szErrMsg);
				}
			}
			else
				break;
		
			// get the version Number
			pCh = egStrtok(pCh, token, " \t\r\n");
			if ( pCh != NULL)
			{
				// copy version Number
				strcpy(LMData[EgLMTotalLicenseCount-1].version, token);

				if ( giVerify)
				{
					sprintf(szErrMsg,"Version: %s", token);
					logTrace(szErrMsg);
					printf("\n%s\n", szErrMsg);
				}

			}
			else
				break;

			// get the expire date
			pCh = egStrtok(pCh, token, " \t\r\n");
			if ( pCh != NULL)
			{
				if ( giVerify)
				{
					sprintf(szErrMsg,"Expiry Date: %s", token);
					logTrace(szErrMsg);
					printf("\n%s\n", szErrMsg);
				}

				strcpy(expDate, token);
				if ( stricmp("unlimited", token) == 0 )
					LMData[EgLMTotalLicenseCount-1].expDate = 0;
					else
					{
						dd = (token[0] - '0' ) * 10 + token[1] - '0';
						mm = (token[3] - '0' ) * 10 + token[4] - '0';
						yyyy = atoi(token+6);
						hh = 23;
						min = 59;
						ss = 59;
						LMData[EgLMTotalLicenseCount-1].expDate = egConvertTime(dd, mm, yyyy, hh, min, ss);

						// copy expire date
						//strcpy(LMData[EgLMTotalLicenseCount].expDate
					}
			}
			else
				break;
			// get the	maxuser
			pCh = egStrtok(pCh, token, " \t\\\r\n");
			if ( pCh != NULL)
			{
				if ( giVerify)
				{
					sprintf(szErrMsg,"MaxUser: %s", token);
					logTrace(szErrMsg);
					printf("\n%s\n", szErrMsg);
				}

				// copy Max User
				if ( stricmp(token, "unlimited") == 0 )
					LMData[EgLMTotalLicenseCount-1].maxUsers = -1;
				else
					LMData[EgLMTotalLicenseCount-1].maxUsers = atoi(token);
			}
			else
				break;

			// get the	Host Id
			pCh = egStrtok(pCh, token, " \t\r\n");

			
			if ( pCh != NULL)
			{
				sprintf(szErrMsg,"ID Token=%s", pCh);			
				logTrace(szErrMsg);

				switch(token[8])
				{
					case HID_MACID_SIG_U:
					case HID_MACID_SIG_L:
						LMData[EgLMTotalLicenseCount-1].hostIdType = HID_MACID;
						strcpy(LMData[EgLMTotalLicenseCount-1].hostId, token+9);
						
						break;

					case HID_DISKID_SIG_U:
					case HID_DISKID_SIG_L:
						LMData[EgLMTotalLicenseCount-1].hostIdType = HID_DISKID;
						strcpy(LMData[EgLMTotalLicenseCount-1].hostId, token+9);

						break;

					case HID_DIALOGIC_SIG_U:
					case HID_DIALOGIC_SIG_L:
						LMData[EgLMTotalLicenseCount-1].hostIdType = HID_DIALOGIC;
						strcpy(LMData[EgLMTotalLicenseCount-1].hostId, token+9);

						break;

					case HID_NMS_SIG_U:
					case HID_NMS_SIG_L:
						LMData[EgLMTotalLicenseCount-1].hostIdType = HID_NMS;
						strcpy(LMData[EgLMTotalLicenseCount-1].hostId, token+9);

					case HID_ShCti_SIG_U:
					case HID_ShCti_SIG_L:
						LMData[EgLMTotalLicenseCount-1].hostIdType = HID_ShCti;
						strcpy(LMData[EgLMTotalLicenseCount-1].hostId, token+9);

						break;

					default:

						LMData[EgLMTotalLicenseCount-1].hostIdType = HID_NONE;
						strcpy(LMData[EgLMTotalLicenseCount-1].hostId, "");
				}

				if ( giVerify)
				{
					sprintf(szErrMsg,"Host Id: [%c] %s", token[8], LMData[EgLMTotalLicenseCount-1].hostId);
					logTrace(szErrMsg);
					printf("\n%s\n", szErrMsg);			
				}

			}
			else
				break;
			
			// get the	Key
			pCh = egStrtok(pCh, token, " \t\r\n");
			if ( pCh != NULL)
			{
					strcpy(LMData[EgLMTotalLicenseCount-1].key, token+4);
			}
			else
				break;

			strcpy(LMData[EgLMTotalLicenseCount-1].userDefinedTokens, pCh);

			sprintf(szBuf,"License Deatil [%d]:\nProductid=%s, Version=%s, expDate=%s, maxUsers=%d\nKey=%s,HostId=[%d]%s,Tokens=%s",
				EgLMTotalLicenseCount-1,				
				LMData[EgLMTotalLicenseCount-1].productId,
								LMData[EgLMTotalLicenseCount-1].version,
								expDate,
								LMData[EgLMTotalLicenseCount-1].maxUsers,
								LMData[EgLMTotalLicenseCount-1].key,
								LMData[EgLMTotalLicenseCount-1].hostIdType,
								LMData[EgLMTotalLicenseCount-1].hostId,
								LMData[EgLMTotalLicenseCount-1].userDefinedTokens);

			logTrace(szBuf);

			if ( giVerify)
			{
				printf("\n%s\n", szBuf);
			}
	
		}// while
	}// while
	// close file
	fclose(fp);

//gXXX=EgLMTotalLicenseCount;

	sprintf(szErrMsg,"Valid Licence Found= %d",EgLMTotalLicenseCount);
	logTrace(szErrMsg);
	
#ifdef TESTOUT
            fprintf(stderr, "%s\n", szErrMsg);
#endif
	 return 1;
}



/****************************************************************
** Function Name: egGetToken
** Purpose: 
** Algorithm:
**
			return EGLM_NOENTRYFOUND;
			return EGLM_OK;
			return EGLM_INVALID;

** 
******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"egGetToken"
int egGetToken(char * productId, char *tokenName, char *tokenValue)
{
	int i;
	char *pCh;
	char token[128];
	char token1[128];
	int len;

//	EgLMTotalLicenseCount = gXXX;

	len = strlen(tokenName);
	if (len == 0)
		return EGLM_NOENTRYFOUND;

	for ( i = 0; i < EgLMTotalLicenseCount; i++)
	{
		if ( stricmp(LMData[i].productId, productId) == 0)
			break;
	}

	if ( i == EgLMTotalLicenseCount)
		return EGLM_NOENTRYFOUND;

	pCh = LMData[i].userDefinedTokens;

	pCh = egStrtok(pCh, token, " ");

	while ( pCh != NULL)
	{
		strcpy(token1, token);
		
		token1[len] = '\0';

		if ( stricmp(token1, tokenName) == 0 )
		{
			strcpy(tokenValue, token+len+1);
			return EGLM_OK;
		}

		pCh = egStrtok(pCh, token, " ");

	}
	return EGLM_INVALID;
}




/****************************************************************
** Function Name: egGetVersion
** Purpose: 
** Algorithm:
**
			return NULL;

** 
******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"egGetVersion"
char * egGetVersion(char *productId)
{
	int i;
//	char *pCh;
//	char token[128];
//	int len;

	for ( i = 0; i < EgLMTotalLicenseCount; i++)
	{
		if ( stricmp(LMData[i].productId, productId) == 0)
		{
			return (LMData[i].version);
		}
	}

	return NULL;
}



/****************************************************************
** Function Name: egGetCount
** Purpose: 
** Algorithm:
**
			return NULL;

** 
******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"egGetCount"
int egGetCount(char *productId)
{
	int i;
//	char *pCh;
//	char token[128];
//	int len;

	for ( i = 0; i < EgLMTotalLicenseCount; i++)
	{
		if ( stricmp(LMData[i].productId, productId) == 0)
		{
			return (LMData[i].maxUsers);
		}
	}

	return 0;
}









/****************************************************************
** Function Name: egLMGetValidEntry
** Purpose: To get the Product entry from the license file
**
** Algorithm:
**
** 
******************************************************************/
#undef __FNNAME__
#define __FNNAME__	"egLMGetValidEntry"
int egLMGetValidEntry(FILE *fp, char * buf)
{
	char rBuf[1024];
	char * pCh;
	char token[128];
	bool lineFound;
	int lastIndex;
	char *pch1;



	buf[0] = '\0';
	pCh = fgets(rBuf, sizeof(rBuf) -1, fp);

	lineFound = false;
	while (lineFound == false)
	{		
#ifdef TESTOUT
	fprintf(stderr, "Read Buffer=[%s]\n", rBuf);
#endif

		egStrip(rBuf);
		pCh = rBuf;
		// check if there is any keyword "EGLM"
		//pCh = strrchr(rBuf, '\\');

		pCh = egStrtok(pCh, token, " \\");

	
		if ( pCh != NULL )
		{
#ifdef TESTOUT
	fprintf(stderr, "Next Line Indication Found token=%s\n", token);
#endif
			if ( stricmp(token, "EGLM") == 0 )
			{
#ifdef TESTOUT
	fprintf(stderr, "EGLM token found\n");
#endif
				while(true)
				{
					
					pch1 = strrchr(rBuf, '\r');
					if ( pch1 != NULL)
						*pch1 = '\0';
					pch1 = strrchr(rBuf, '\n');
					if ( pch1 != NULL)
						*pch1 = '\0';

					pch1 = strrchr(rBuf, '\\');

					/*lastIndex = strlen(rBuf);

					if ( rBuf[lastIndex -1] == '\r' || rBuf[lastIndex-1] == '\n')
					{
						rBuf[lastIndex -1] = '\0';
						lastIndex--;
					}

					if ( rBuf[lastIndex -1] == '\\')
					{
						rBuf[lastIndex - 1] = ' ';
						strcat(buf, rBuf);
					}
*/
					if ( pch1 != NULL)
					{
						*pch1 = ' ';
						strcat(buf, rBuf);

					} 
					else
					{
						strcat(buf, rBuf);
						lineFound = true;			
						break;
					}
#ifdef TESTOUT
	fprintf(stderr, "Buf=[%s]\n", buf);
#endif

					pCh = fgets(rBuf,sizeof(rBuf) -1, fp);

					if ( pCh == NULL)
					{
						lineFound = true;			
						break;
					}
					egStrip(rBuf);
#ifdef TESTOUT
	fprintf(stderr, "Read Buffer=[%s]\n", rBuf);
#endif

				}//while
			}
		}
		pCh = fgets(rBuf, sizeof(rBuf) -1, fp);
		if ( pCh == NULL)	
			if ( feof(fp))
				break;
	}
	

	if ( buf[0] == '\0')
		return 0;
	else
		return 1;
}





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




void logTrace(char *msg)
{
	time_t curTime;	
	struct tm *cTm;
	FILE *fp;

	curTime = time(NULL);
	cTm = localtime(&curTime);

	if ( (fp = fopen(gszTraceFileName, "a+t")) == NULL)
		return;
	else
	{
		fprintf(fp,"%02u/%02u/%04u %02u:%02u:%02u\t::%s\n", 
						cTm->tm_mday,
						cTm->tm_mon+1,
						cTm->tm_year+1900,
						cTm->tm_hour,
						cTm->tm_min,
						cTm->tm_sec,
						msg);

		fclose(fp);

	}
	return;

}




/* 
* get dialogic board serial No
*/
int getDialogicBS(char *szSln)
{
   szSln[0] = '\0';
   return 0;
}




/*
* Get NMS board Serail NUmber 
* return -1 for error
*         0: for success
*/
int getNMSBS(char *pszSlNo)
{
    pszSlNo[0] = '\0';

    return 0;
}



/* 
* get dialogic board serial No
*/

int GetShCtiPath(char *pszPath);

/* 
* get dialogic board serial No
*/
int getShCtiBS(char *szSln)
{

   szSln[0] = '\0';
   return 0;
}




/***********************************************************/

#undef __FNNAME__
#define __FNNAME__	"egStrip"
char * Strip(char *x, char chr )
{
	char *pch = x;
	char *pchl;

	if ( x == NULL )
		return NULL;

	if ( *x == '\0')
		return NULL;

	while(*pch == chr && *pch != '\0')
	{
		pch++;
	}
	
	pchl = x + strlen(x) -1;

	while(*pchl == chr && pchl > pch)
	{
		pchl--;
	}
	

	int i =0;

	if ( pch != x )
	{
		while(pch <= pchl )
		{
			x[i] = *pch;
			i++;
			pch++;
		}
		x[i] = '\0';
	}
	else
	{
		pchl++;
		*pchl = '\0';
	}

	if ( *x == '\0')
		return NULL;
	else
		return x;
}

/****************************************************/

char * Strtok( char * pCh, char *token, char * sep)
{

	int i = 0;
	int j;
	int sepLen = strlen(sep);


	token[0] ='\0';

	if ( *pCh == '\0' )
		return NULL;


	while (*pCh != '\0')
	{
		for (j = 0; j < sepLen; j++)
		{
			if (*pCh == sep[j])
				break;
		}
		
		if ( j < sepLen )
			break;
		token[i] = *pCh;
		pCh++;
		i++;
	}
	token[i] = '\0';

	while (*pCh != '\0')
	{
		for (j = 0; j < sepLen; j++)
		{
			if (*pCh == sep[j])
				break;
		}
		if ( j == sepLen)
			break;
		pCh++;
	}

	return pCh;
}


/***********************************************************/
unsigned int ConvertTime(int dd, int mm, int yy, int hh, int min, int ss)
{
   struct tm mTime;
   unsigned int n;

   mTime.tm_sec = ss;
   mTime.tm_min = min;
   mTime.tm_hour = hh;

   mTime.tm_mday = dd;
   mTime.tm_mon = mm-1;
   if ( yy > 1900)
      mTime.tm_year = yy - 1900;
   else
      mTime.tm_year = yy;
   mTime.tm_isdst = 0;
   
   if ( (n = mktime(&mTime)) == -1)
   {
      return -1;
   }
   return n;
}
