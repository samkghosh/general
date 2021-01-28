/*
* File Name: copyfile.cpp
* Part of Easy Library
* Copyright @ 2001
*/

#include <stdio.h>
#include <string.h>
#include <io.h>
#include <errno.h>
#include <direct.h>


#include "easy.h"



/*
* Copy function
*/

int egCopyFile(char *source, char *dest)
{

		FILE *sFp;
	FILE *dFp;
	unsigned ret;
	char buffer[4*1024];


	if ( (sFp = fopen(source, "r+b")) == NULL)
	{
#ifdef DEBUG
		TRACE("File Open Failed %s [%d]\n", source, errno);
#endif
		return -1;
	}

	if ( (dFp = fopen(dest, "w+b")) == NULL)
	{
#ifdef DEBUG

		TRACE("File Open Failed %s [%d]\n", dest, errno);
#endif

		fclose(sFp);
		return -1;
	}


	do
	{
		 ret = fread(buffer,1, sizeof(buffer), sFp);
		 
		if (ferror(sFp) )
		{
			fclose(dFp);
			_unlink(dest);
			fclose(sFp);
#ifdef DEBUG

			TRACE("Read Error %d\n", errno);
#endif
			return -1;
		}

		// write content
		if ( ret > 0 )
		  if (fwrite(buffer,1, ret,dFp) != ret )
		  {
			fclose(dFp);
			_unlink(dest);
			fclose(sFp);
#ifdef DEBUG
			TRACE("Write Error %d\n", errno);
#endif
			return -1;
		  }
	}while(!feof(sFp));

	fclose(sFp);

	fclose(dFp);

	return 0;

}
