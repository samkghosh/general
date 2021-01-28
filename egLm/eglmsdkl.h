/***************************************************************
** eglmsdkl.h
**
** Purpose:
**
** Copyright (C), 2002, Alliance Infotech (P) Ltd.
** Revision History:
**
** [0]	11-07-2002	SG	Created
**
****************************************************************/


#ifndef __EGLMSDKL_H__
#define __EGLMSDKL_H__
#include <stdio.h>
#define MAXLICENSE		50

#define HID_NONE		0
#define HID_MACID		1
#define HID_DISKID		2
#define HID_DIALOGIC	3
#define HID_NMS			4
#define HID_ShCti		5


#define HID_MACID_SIG_U	'M'
#define HID_MACID_SIG_L	'm'

#define HID_DISKID_SIG_U	'D'
#define HID_DISKID_SIG_L	'd'

#define HID_DIALOGIC_SIG_U	'I'
#define HID_DIALOGIC_SIG_L	'i'

#define HID_NMS_SIG_U		'N'
#define HID_NMS_SIG_L		'n'

#define HID_ShCti_SIG_U		'S'
#define HID_ShCti_SIG_L		's'


typedef unsigned char  T_Eaddr[32];

int egLMgetLMDataFlName(char *flName);
int egLMgetProductEntry(char * lmFileName, char * productId, char * buf);
int egGetKey(char *buf, char *key);
bool egLMVerifyKey(char *buf);
int getEtherAddr(T_Eaddr *);
int getDiskSerialNum(char * slNum);
int egLMGetValidEntry(FILE *fp, char * buf);
int egInitLm(char * lmFileName);
int egLMCheck(char * productId);
int egGetToken(char * productId, char *tokenName, char *tokenValue);

char * egGetVersion(char *productId);

int egGetCount(char *productId);

void GetMACaddress(void);


typedef struct 
{
	char productId[32];
	char version[32];
	unsigned long expDate;
	int  maxUsers;
	int  currUsers;
	char key[32];
	char hostId[32];
	int hostIdType;		//1: mcId, 2: disk sl no, 3: procId

    char userDefinedTokens[256];
}T_LMData;



int getDialogicBS(char *szSln);
int getNMSBS(char *pszSlNo);


#endif