/*
* File Name: strtok.cpp
* strtok function with thread save
* part of easy library
* copyright @ 2001
*/

#include <stdio.h>
#include <string.h>

#include "easy.h"


char * egStrtok( char * pCh, char *token, char * sep)
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