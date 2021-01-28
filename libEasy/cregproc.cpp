// cregproc.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include "cRegProcess.h"
#include <conio.h>
#include <winsock2.h>
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool bMode;

HKEY appKey;

static int OpenKey(HKEY pKey,char* appName,HKEY* newHKey);

static 	int SetkValue(HKEY secKey,char* key,char* value);

static 	int SetkValue(HKEY secKey,char* key,unsigned long value);
	
static 	int GetkValue(HKEY secKey,char* key,char* value);

static	int GetkValue(HKEY secKey,char* key,unsigned long * value);


/*CRegProcess::CRegProcess()
{

}*/

CRegProcess::CRegProcess(char* appName, bool bWrite)
{
	int ret;

	bMode = bWrite;
	if( strcmp(appName,"") != 0)
	{
		strcpy(aName,appName);
		ret = CreateAppKey(aName);
	}
	else
		appKey = 0;

}




CRegProcess::~CRegProcess()
{
	LONG ret;
	if( appKey != 0)
		ret = RegCloseKey(appKey);	
}

int CRegProcess::CreateAppKey(char* appName)
{
	long ret = 0;
	
	HKEY hKey = 0;
	
	DWORD lDisp;

	char	subKey[256];

	try
	{

			if(strcmp(appName,"") == 0)
				return FAIL;

			sprintf(subKey,"Software\\%s",appName);

			if ( bMode)
			{
				ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
									subKey,
									0,
									"",
									REG_OPTION_NON_VOLATILE,
									KEY_ALL_ACCESS,
									NULL,
									&hKey,
									&lDisp);
			}
			else
			{
				ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
									subKey,
									0,
									"",
									REG_OPTION_NON_VOLATILE,
									KEY_READ,
									NULL,
									&hKey,
									&lDisp);

			}

			if(ret != ERROR_SUCCESS)
				return FAIL;
			else
			{
				appKey = hKey;
				
			}
	}
	catch(...)
	{
		return FAIL;
	}

	return SUCCESS;
}



int CRegProcess::CreateSectionKey(char* newSection)
{

	long ret = 0,ret1 = 0;
	
	HKEY hKey;
	
	char	subKey[256];

	try
	{
	
		if (strcmp(aName,"") == 0 || strcmp(newSection,"") == 0)
			return FAIL;

		if( appKey == 0)
		{
			if( strcmp(aName,"") == 0)
				return FAIL;

			ret = CreateAppKey(aName);
			
		}
		

		sprintf(subKey,"%s",newSection);

		ret = OpenKey(appKey,subKey,&hKey);

		ret1 = RegCloseKey(hKey);
	}
	catch(...)
	{
		return FAIL;
	}

	return ret ;
}


int CRegProcess::DeleteSectionKey(char* section)
{

	return SUCCESS;

}


int CRegProcess::CreateSubKey(char* section,char* newKey,char* value)
{
	long ret = 0,ret1 = 0;

	HKEY secKey;

	try
	{
	
		if (strcmp(aName,"") == 0 || strcmp(section,"") == 0 || strcmp(newKey,"") == 0)
			return FAIL;

		if( appKey == 0)
		{
			ret = CreateAppKey(aName);
		}

		ret = OpenKey(appKey,section,&secKey);

		if(secKey != 0)
		{
			ret = SetkValue(secKey,newKey,value);		
			
			ret1 = RegCloseKey(secKey);
		}
		else
			return FAIL;
	}
	catch(...)
	{
		return FAIL;
	}

	return ret;
}


int CRegProcess::DeleteSubKey(char* section,char* key)
{

	return SUCCESS;
}


int CRegProcess::SetKeyValue(char* section,char* key,char* value)
{
	long ret=0,ret1 = 0;

	HKEY secKey;

	try
	{

		if (strcmp(aName,"") == 0 || strcmp(section,"") == 0 || strcmp(key,"") == 0)
			return FAIL;

		if( appKey == 0)
		{
			ret = CreateAppKey(aName);
			
		}

		ret = OpenKey(appKey,section,&secKey);

		if(secKey != 0)
		{
			ret = SetkValue(secKey,key,value);
			ret1 = RegCloseKey(secKey);
		}
		else
			return FAIL;
	}
	catch(...)
	{
		return FAIL;
	}


	return ret;
}

/*
*
*/

int CRegProcess::SetKeyValue(char* section,char* key,unsigned long value)
{
	long ret=0,ret1 = 0;

	HKEY secKey;

	try
	{

		if (strcmp(aName,"") == 0 || strcmp(section,"") == 0 || strcmp(key,"") == 0)
			return FAIL;

		if( appKey == 0)
		{
			ret = CreateAppKey(aName);
			
		}

		ret = OpenKey(appKey,section,&secKey);

		if(secKey != 0)
		{
			ret = SetkValue(secKey,key,value);
			ret1 = RegCloseKey(secKey);
		}
		else
			return FAIL;
	}
	catch(...)
	{
		return FAIL;
	}


	return ret;
}

int CRegProcess::GetKeyValue(char* section,char* key,char* value)
{
	long ret = 0,ret1 = 0;

	HKEY secKey;

	try
	{
		if (strcmp(aName,"") == 0 || strcmp(section,"") == 0 || strcmp(key,"") == 0)
			return FAIL;

		if( appKey == 0)
		{
			ret = CreateAppKey(aName);
			
		}

		ret = OpenKey(appKey,section,&secKey);

		if(secKey != 0)
		{
			ret = GetkValue(secKey,key,value);
			ret1 = RegCloseKey(secKey);
		}
		else
			return FAIL;
	}
	catch(...)
	{
		return FAIL;
	}

	return ret;

	
}


int CRegProcess::GetKeyValue(char* section,char* key,unsigned long * value)
{
	long ret=0,ret1=0;

	HKEY secKey;

	try
	{
		if (strcmp(aName,"") == 0 || strcmp(section,"") == 0 || strcmp(key,"") == 0)
			return FAIL;

		if( appKey == 0)
		{
			ret = CreateAppKey(aName);
			
		}

		ret = OpenKey(appKey,section,&secKey);

		if(secKey != 0)
		{
			ret = GetkValue(secKey,key,value);
			ret1 = RegCloseKey(secKey);
		}
		else
			return FAIL;
	}
	catch(...)
	{
		return FAIL;
	}

	return ret;

	
}
///Private Functions
//Opening any Key

//int CRegProcess::
static int OpenKey(HKEY pKey,char* appName,HKEY* newHKey)
{

	long ret = 0;
	
	DWORD lDisp;

	try
	{

		if ( bMode)
		{
			ret = RegCreateKeyEx(pKey,
								appName,
								0,
								"",
								REG_OPTION_NON_VOLATILE,
								KEY_ALL_ACCESS,
								NULL,
								newHKey,
								&lDisp);
		}
		else
		{
			ret = RegCreateKeyEx(pKey,
								appName,
								0,
								"",
								REG_OPTION_NON_VOLATILE,
								KEY_READ,
								NULL,
								newHKey,
								&lDisp);


		}
		if(ret != ERROR_SUCCESS)
			return FAIL;
		
	}
	catch(...)
	{
		return FAIL;
	}
	
	return SUCCESS;
}



//int CRegProcess::
static int SetkValue(HKEY secKey,char* key,char* value)
{
	long ret = 0;
//	char lpValue[256];

	try
	{

//		if(value == NULL)
//			lpValue[0]='\0';
//		else
//			strcpy(lpValue,value);

//		ret = RegSetValueEx(secKey,
//							key,
//							0,
//							REG_SZ,
//							(unsigned char *)lpValue,
//							sizeof(lpValue)
//							);

		ret = RegSetValueEx(secKey,
							key,
							0,
							REG_SZ,
							(LPBYTE)value,
							strlen(value) +1
							);
		
		if(ret != ERROR_SUCCESS)
			return FAIL;
	}
	catch(...)
	{
		return FAIL;
	}

	return SUCCESS;


}


//int CRegProcess::
static int SetkValue(HKEY secKey,char* key,unsigned long value)
{
	long ret = 0;
//	char lpValue[256];

	try
	{

//		if(value == NULL)
//			lpValue[0]='\0';
//		else
//			strcpy(lpValue,value);
//			sprintf(lpValue,"%lu",value);

		ret = RegSetValueEx(secKey,
							key,
							0,
							REG_DWORD,
//							(unsigned char *)lpValue,
							(LPBYTE) &value,
							sizeof(value)
							);
	
		if(ret != ERROR_SUCCESS)
			return FAIL;
	}
	catch(...)
	{
		return FAIL;
	}

	return SUCCESS;


}
/*
*
*/

//int CRegProcess::
static int GetkValue(HKEY secKey,char* key,char* value)
{
	long ret = 0;
	
	char lpValue[256];

	DWORD dwl=256;
	DWORD dwType;
	
	try
	{


		lpValue[0] ='\0';

		
		ret = RegQueryValueEx(secKey,
								key,
								NULL,
								&dwType,
								(unsigned char *)lpValue, 
								&dwl
								);

		
		if (ret == (DWORD)ERROR_SUCCESS) 
		{
			switch(dwType)
			{
			case REG_SZ:
				sprintf(value,"%s",lpValue);
				return SUCCESS; 

			default:
				strncpy(value,lpValue,dwl);
				return SUCCESS;
			}
		}
     

		
	}
	catch(...)
	{
		return FAIL;
	}
	
	return NOTAKEY;
}

/*
*
*/
//int CRegProcess::
static int GetkValue(HKEY secKey,char* key,unsigned long * value)
{
	long ret = 0;
	
	char lpValue[256];

	DWORD dwl=256;
	DWORD dwType;
	
	try
	{


		lpValue[0] ='\0';

		
		ret = RegQueryValueEx(secKey,
								key,
								NULL,
								&dwType,
								(unsigned char *)lpValue, 
								&dwl
								);

		
		if (ret == (DWORD)ERROR_SUCCESS) 
		{
			if ( dwType == REG_DWORD)
				memcpy(value,lpValue, dwl);
			else
				*value = 0;

			return SUCCESS;
		}
		
	}
	catch(...)
	{
		return FAIL;
	}
	
	return NOTAKEY;
}


bool CRegProcess::isOk()
{
	return (appKey!=NULL);
}
