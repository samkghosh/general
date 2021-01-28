/*
* FileName: chkpath.cpp
* Part of easy library
* Copyright @ 2001
*/

#include <stdio.h>
#include <string.h>
#include <io.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <direct.h>


#include "easy.h"

#include "easyl.h"
/*
*
*/

int egCheckPath(char *pathName, bool creatIt)
{
	struct _stat stBuff;


	// check the existance of the
	if ( _stat(pathName, &stBuff) == -1)
	{
		if ( errno == ENOENT )
		{
			if ( creatIt == true)
				return( makeDir(pathName));
		}
		
		egerrno = errno;
		return -1;
			
	}
	
	if ( stBuff.st_mode & _S_IFDIR )
	{
		return 0;
	}
	else
	{
		egerrno = _EG_NODIR;
		return -1;
	}

}

