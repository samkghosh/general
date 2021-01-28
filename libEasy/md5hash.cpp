/*
* Filename md5hash.xpp
* Part of Easy Library
* Copyright @ 2003
*/


#include <stdio.h>
#include <string.h>
#include "md5global.h"
#include "md5.h"

int egMd5Hash(char *input, unsigned char *output)
{
	unsigned char out[16];

	MD5_CTX md5Context;


	if ( input == NULL)
		return -1;


	

	// init context
	MD5Init(&md5Context);
	
	MD5Update(&md5Context, (unsigned char *)input, strlen(input));

	MD5Final(out, &md5Context);

	memcpy(output, out, sizeof(out));

	output[16] = '\0';

	return 0;
}



int egMd5Hash(char *input, char *output)
{
	unsigned char out[16];

	MD5_CTX md5Context;


	if ( input == NULL)
		return -1;


	

	// init context
	MD5Init(&md5Context);
	
	MD5Update(&md5Context, (unsigned char *)input, strlen(input));

	MD5Final(out, &md5Context);

	memcpy(output, out, sizeof(out));

	sprintf(output,"%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",
								out[0],
								out[1],
								out[2],
								out[3],
								out[4],
								out[5],
								out[6],
								out[7],
								out[8],
								out[9],
								out[10],
								out[11],
								out[12],
								out[13],
								out[14],
								out[15]);


	return 0;
}

/*
*
*/
void egChangeSeed(unsigned long x)
{
	changeContext(x);

	return;
	
}