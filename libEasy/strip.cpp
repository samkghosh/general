/*
* Filename: strip.cpp
* to strip a specified character
* part of easy library
* copyright @ 2001
*/

#include <stdio.h>
#include <string.h>

#include "easy.h"



/* 
* Strips the char from both the sides
*/

#undef __FNNAME__
#define __FNNAME__	"egStrip"
char * egStrip(char *x, char chr )
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



/*
* Strips the cahr from the left
*/
#undef __FNNAME__
#define __FNNAME__	"egStripl"
char * egStripl(char *x, char chr)
{
	char *pch = x;
	char *pchl;


	if ( x == NULL)
		return NULL;

	if ( *x == '\0')
		return NULL;

	while(*pch == chr && *pch != '\0')
	{
		pch++;
	}
	
	pchl = x + strlen(x) -1;
	

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

	if ( *x == '\0')
		return NULL;
	else
		return x;
}



/*
* Strips the cahr from the right
*/
#undef __FNNAME__
#define __FNNAME__	"egStripr"
char * egStripr(char *x, char chr)
{
	char *pchr;

	if (x == NULL )
		return NULL;

	if ( *x == '\0' )
		return x;
	
	pchr = x + strlen(x) -1;

	while(*pchr == chr && pchr > x)
	{
		pchr--;
	}
	
	pchr++;
	*pchr = '\0';
	
	if ( *x == '\0')
		return NULL;
	else
		return x;
}

