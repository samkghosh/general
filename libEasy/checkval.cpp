/*
* Filename: checkval.cpp
* To check a value is in the set or range
* Part of easy library
* copyright @ 2001
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "easy.h"


static bool isNumber(char *pszBuf);

bool egCheckValue(long va, char * range)
{
	char x[32];
	
	sprintf(x,"%ld", va);
	
	return(egCheckValue(x,range));

}


bool egCheckValue(char * va, char * range)
{

	char *pch, *tpch;
	char token[128], subtoken[128], nextSubToken[128];
	long x,y,z;
	bool bCheckAlpa=false;


	if ( isNumber(va) == true)
	{
		z = atol(va);
	}
	else
		bCheckAlpa = true;

	pch = range;

	pch = egStrtok(pch, token, ",");
	
	while (pch != NULL)
	{
		tpch = token;
		
		tpch = egStrtok(tpch,subtoken, "-");
		if ( (tpch = egStrtok(tpch, nextSubToken, " -")) == NULL)
		{
			egStrip(token);

			if ( bCheckAlpa == true || isNumber(token) == false)
			{
				if ( stricmp(va, token) == 0 )
					return true;
			}
			else
			{
				x = atol(token);
				if (x == z )
					return true;
			}
		}
		else
		{
			if ( bCheckAlpa == true || isNumber(subtoken) == false || isNumber(nextSubToken) == false)
			{
				if ( stricmp(va, subtoken) >=0 && stricmp(va, nextSubToken) <=0)
					return true;
			}
			else
			{
				x = atol(subtoken);
				y = atol(nextSubToken);
				if  (z >= x && z <= y)
					return true;
			}
		}
		pch = egStrtok(pch,token,",");
	}
	
	return false;
}


/*
*
*/
static bool isNumber(char *pszBuf)
{
	char *pch = NULL;

	if ( pszBuf == NULL)
		return false;

	if (pszBuf[0] == '\0')
		return false;


	pch = pszBuf;

	while(*pch != '\0')
	{
		if ( isdigit(*pch) == 0)
			return false;
		pch++;
	}

	return true;
}
