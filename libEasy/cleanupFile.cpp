/******************************************************************************************************************************************
*File Name : EGDDMApp.cpp
*
*purpose	  : Implements class CEGDDMApp, which is used to purge old files.
*
* History:
*	[00]	[xx]	[GK]	Created
*
************************************************************************************************************************************************/
#include <stdio.h>
#include <io.h>
#include <time.h>
#include <windows.h>
#include <process.h>


#include "cleanupfile.h"

//void					ThreadInterpreter ( void*);	//thread to interpret each line
//bool					g_bThread ;
//char					g_szInputFileName[260] ;
/*************************************************************************************************************************
Proc Name :CEGDDMApp
Purpose :
	All initializations are done in this function
**************************************************************************************************************************/
CEGDDMApp::CEGDDMApp(void)
{
	//set error flag to false.
	m_bErrorFlag = false;		//set error flag to false.
	
	//initialize memory buffers
	memset( m_szError, '\0', sizeof( m_szError)) ;	
	memset( m_szInputFileName, '\0', sizeof( m_szInputFileName)) ;	
	memset( m_szCurPath, '\0', sizeof( m_szCurPath)) ;	
	memset( m_szDestPath, '\0', sizeof( m_szDestPath)) ;
	memset( m_szCurExtention, '\0', sizeof( m_szCurExtention)) ;	
	memset( m_szLogFileName, '\0', sizeof( m_szLogFileName)) ;
	
	//set default parameters
	m_lDeleteDaysBefore = DEFAULT_DELETEBEFOREDAYS ;	//set default hour
	m_lPollHour = DEFAULT_POLLTIME_HOURS ;				//set default poll time.
	sprintf( m_szCurPath, "%s", DEFAULT_CURRENTPATH) ;	//set default current path.
	
	//clear the command info buffer.
	//memset(m_CommandInfo, '\0', sizeof(m_CommandInfo) * MAX_COMMANDS_NO) ;
	
	//set all command names.
	
	sprintf( m_CommandInfo[0].szCommand, "SETPATH") ;
	sprintf( m_CommandInfo[1].szCommand, "SETPOLLTIME") ;
	sprintf( m_CommandInfo[2].szCommand, "SETDELBEFORE") ;
	sprintf( m_CommandInfo[3].szCommand, "SETEXTENSION") ;
	sprintf( m_CommandInfo[4].szCommand, "DELETEFILES") ;
	sprintf( m_CommandInfo[5].szCommand, "LOGFILENAME") ;
	sprintf( m_CommandInfo[6].szCommand, "SETDESTPATH");
	sprintf( m_CommandInfo[7].szCommand, "COPYFILES");
	sprintf( m_CommandInfo[8].szCommand, "RECURSION");


	//m_hLogFile = NULL ;		//set input log file to NULL.

}



/*************************************************************************************************************************************************************
Proc Name	:	SetLogFileName
Purpose		:	Set member logpath var and open the log file.
Input		:	pszLogFileName, pointer to log file name.
Possible Errors : Invalid file name.
return value	: -1, invalid input
				  -2, unable to open input file.
				  1, success.
NOTE:	member var for log path is not needed, so we should remove it.
**************************************************************************************************************************************************************/
int CEGDDMApp::SetLogFileName(char* pszLogFileName)
{

	try
	{
		if( pszLogFileName == NULL)	
		{
			m_bErrorFlag = true ;
			sprintf( m_szError, "Invalid log file was passed in SetLogFileName") ;
			WriteLog(m_szError);
			return -1;
		}

		if( strlen(pszLogFileName) == 0)
		{
			m_bErrorFlag = true ;
			sprintf( m_szError, "Invalid log file was passed in SetLogFileName") ;
			WriteLog(m_szError);
			return -1;
		}

		sprintf( m_szLogFileName, "%s\\%s%u.txt",pszLogFileName,"EGDDM",time(NULL)) ;	//set log file name

		
		return 1 ;
	}
	catch(...)
	{
		sprintf(m_szError,"UnHandeled Exception in SetLogFileName function");
		WriteLog(m_szError);
		return 1;
	}
}

/********************************************************************************************************************************************************
Processing :
	a) Open log file.
	b) Write line into it.
	c) close log file.
*********************************************************************************************************************************************************/
bool CEGDDMApp::WriteLog(char* pszLog)
{
	FILE		*fp ;						//file handle

	try
	{

		if((*m_szLogFileName == '\0') || strlen(m_szLogFileName) <= 0)
		{
			return false ;
		}
		fp = fopen( m_szLogFileName, "ab") ;	//open input file
		if( fp == NULL)							//unable to open log file
		{
			return false;
		}
		
		fwrite( pszLog, sizeof(char), strlen(pszLog), fp) ;		//write to log file.
		fwrite( "\r\n", sizeof(char), strlen("\r\n"),fp) ;
		WriteLog(pszLog);
		
		fclose(fp) ;							//close log file.
		return true;
	}
	catch(...)
	{
		sprintf(m_szError,"UnHandeled Exception in WriteLog function");
		WriteLog(m_szError);
		return 1;
	}

}
/*************************************************************************************************************************************
Delete any var if memory is allocated
******************************************************************************************************************************************/
CEGDDMApp::~CEGDDMApp(void)
{
	
}


/******************************************************************************************************************************************
Proc Name : SetCurrentPath
Purpose : Sets current path to the supplied path.
***********************************************************************************************************************************************/
void	CEGDDMApp::SetCurrentPath( char*pszPath) 		//set current path 
{
	char		szLog[260];
	try
	{

		if( pszPath == NULL)	//check input path should not be NULL
		{
			m_bErrorFlag = true ;
			sprintf( m_szError, "Supplied Input path is not a valid path");
			WriteLog(m_szError);
			return ;
		}
		sprintf( m_szCurPath, pszPath) ;	//copy input path to member var
	}
	catch(...)
	{
		sprintf(szLog,"UnHandeled Exception in SetCurrentPath function");
		WriteLog(szLog);
		return ;
	}
	
		
}



/***********************************************************************************************************************************************
Proc Name	: SetPollingTime
Purpose		: Sets polling time to the given time.

**************************************************************************************************************************************************/
void	CEGDDMApp::SetPollingTime( char* pszHours)				//set polling time in hours.
{

	m_lPollHour = atol(pszHours) ;
}




/*****************************************************************************************************************************************************
Proc Name	: SetDeleteHourBefore
Purpose		: Sets hours , All files accessed before this hour will be deleted.

**************************************************************************************************************************************************************/
bool	CEGDDMApp::SetDeleteDaysBefore( char* pszDays)
{
	long lDays ;
	
	try
	{
		
		lDays =  atol( pszDays) ;

		if( lDays > MAX_DELDAYS_LIMIT )	//validate input, it should not cross limits
		{
			m_bErrorFlag = true ;
		}

		if( lDays < MIN_DELDAYS_LIMIT )//validate input, it should not cross limits
		{
			m_bErrorFlag = true ;
		}


		m_lDeleteDaysBefore = lDays ;
				
		return true ;
	}
	catch(...)
	{
		m_bErrorFlag = true ;
		sprintf( m_szError, "Unhandled exception in SetDeleteHourBefore") ;
		WriteLog(m_szError);
		return false ;
	}
}

void	CEGDDMApp::ExitCEGDDMApp()
{
	m_bToTerminate = false ;

}





/*********************************************************************************************************************************************************************
Proc Name	: InitCEGDDMApp
Input		: input file name.
Output		: void
Processing	: 
  It will do following tasks, and if error occures, then it will set error flag to true.
  	1. It will set input file name. Then it will validate this file if it exists. 
	2. It will read each line until end of line
		2.1 If this line is commnet then ignore it.
		2.2 If this line is for setting some input parameter then set it.
		2.3 If this is some command, then call it.( only DeleteFile)
Possible Errors
	1. Input file does not exist.
	2. 

*************************************************************************************************************************************************************************/
bool	CEGDDMApp::InitCEGDDMApp(char* pszInputFileName)
{
	char		szLog[250];
	

	sprintf(szLog,"Application EGDDM initiated successfully");
	WriteLog(szLog);
	try
	{


		if( pszInputFileName == NULL)		//validate input file, check pointer
		{
			m_bErrorFlag = true ;
			sprintf( m_szError, "Input file is NULL, please pass a valid pointer to input file name") ;
			WriteLog(m_szError);
			return false ;
		}

		if( strlen(pszInputFileName) <= 0)	//validate input file, check file name length
		{
			m_bErrorFlag = true ;
			sprintf( m_szError, "Input file name is not valid, please pass a valid pointer to input file name") ;
			WriteLog(m_szError);
			return false ;
		}

		//check if file exist
		if( _access( pszInputFileName, 00) )	// 00 is existence permission
		{
			m_bErrorFlag = true ;
			sprintf( m_szError, "Input file does not exist, please pass a valid pointer to valid input file name") ;
			WriteLog(m_szError);
			return false ;
		}

		//g_bThread = true ;
		

		m_bToTerminate = true ;
		sprintf( m_szInputFileName, pszInputFileName) ;
		WriteLog(m_szInputFileName);
		ProcInterpreter() ;

		sprintf(szLog,"Application terminated successfully");
		WriteLog(szLog);
		//_beginthread ( ThreadInterpreter, 0, NULL) ;	//begin thread

		return true ;
	}
	catch(...)
	{
		sprintf( m_szError, "Unhandled exception in InitCEGDDMApp") ;
		WriteLog(m_szError);
		return false ;
	}
}



/****************************************************************************************************************************************************
If the theread is on the g_bThread will be on.
It can be killed from outside by setting g_bThread to false, or by calling Endthread from exitinstance

*************************************************************************************************************************************************************/
void CEGDDMApp::ProcInterpreter ()
{
	
	FILE		*fp ;
	char*		pStream ;
	char		szNextLine[260] ;
	char		szLog[260];
	//open the file

	try
	{

		while( m_bToTerminate == true)	//if g_bThread is false then return
		{
			//open input file.
			fp = fopen( m_szInputFileName, "rt") ;
			if( fp == NULL)	//unable to open file
			{
			//	g_bThread = false ;
				m_bToTerminate = false ;
				//m_bErrorFlag = true ;
				//sprintf( m_szError, "Unable to open input file %s",pszInputFileName) ;
				return ;
			}
			//write a loop to read all input lines.
			while( m_bToTerminate )//&& //( donot awant to terminate fun from outside))
			{
				//read next line
				pStream = fgets( szNextLine, sizeof(szNextLine), fp) ;	//read next line
				//check if EOF has reached.
				if( pStream == NULL )	//end of file reached.
				{
					fclose(fp) ;
					break ;
				}

				ParseLine(szNextLine) ;	//parse line
				
			}//inner while loop
			
			
			fclose(fp) ;	//close file and sleep for time 
			
			for( long i = 0 ; i < m_lPollHour * 3600 ; i = i + 5 ) 
			{
				if( m_bToTerminate == false)
					return ;
				Sleep(5000) ;
			}
				//Sleep( m_lPollHour * 3600) ;
		}//Outer while loop . while( g_bThread == true)	//if g_bThread is false then return
		
		m_bToTerminate = false ;
		//g_bThread = false ;
		return  ;
	}
	catch(...)
	{
		sprintf( szLog, "Unhandled exception in ProcInterpreter") ;
		WriteLog(szLog);
		
	}

}




/**************************************************************************************************************************************************************************
Proc Name: DeleteFileBefore
Purpose:	Delete all files in the m_szCurPath which have
				1. having extention m_szCurExtention, 
				2. have creation time less than current time - m_lDeleteHourBefore 
Processing
	1. Enumerate all files in this folder having current extention.
	2. Check whether it's last access time is less than current time - hours.
	
time64 returns the time elapsed in seconds

*********************************************************************************************************************************************************************************/


void	CEGDDMApp::DeleteFileBefore()
{
	try
	{
		m_bIsDeleteSet = true ;
		ProcessFileBefore(m_szCurPath , m_szDestPath) ;

		return ;
	}
	catch(...)
	{
		sprintf(m_szError , "Unhandeled Exception in DeleteFileBefore Function");
		WriteLog(m_szError);
		return;
	}


#if 0
	_finddata_t				finfo ;
	__time64_t				now ;		//current time (sec elapsed after 1 jan, 1970
	char					szFileName[260] ;
	long					lSecDelBefore ;		//seconds converted from m_lDeleteHourBefore ;
	long					fHandle ;
	int						nLenExt ;
	int						nIndexFileName ;
		
	try
	{

		_time64(&now) ;						//get current time.

		lSecDelBefore = 3600 * 24 * m_lDeleteDaysBefore ;	//convert hours to seconds.

		fHandle = _findfirst( szFileName, &finfo) ;		//find first file
		if( fHandle == -1)	
		{
			return ;		//return successfully, becoz no such file exist
		}
		
		nLenExt = strlen( m_szCurExtention) ;

		do	//repeat for all files
		{

		//this code is commaented, and can be included if we want to chack extention of each file.
			//check whether the file contains extention 
			nIndexFileName = strlen( finfo.name ) ; //get len of file.
			if( nIndexFileName < nLenExt)		//extention should be small always.
				continue ;

			for ( int i = nLenExt ; i > 0; i--)	//check if extention does not match then continue.
			{
				if( i == nLenExt - 2)	//extentin can be of three chars only
					break ;
				if( finfo.name[nIndexFileName - 1] != m_szCurExtention[ i - 1])
				{
					return ;
					//continue ;
				}

				nIndexFileName -- ;
			}
	  


			if( finfo.time_access < now - lSecDelBefore )	//if this file is older than lSecDelBefore
			{
				sprintf( szFileName, "%s\\%s",m_szCurPath, finfo.name) ;
				_unlink(szFileName) ;//delete this file
								
			}


		} while( _findnext( fHandle, &finfo) == 0) ;		//find next file

		return ;
	}
	
#endif
}



/*****************************************************************************************************************************************************************************
Proc Name : Parseline
Purpose	  : This is used to parse a line, the line may be a comment, command, or 
		if first two
Input	
	-1, invalid
	1, comment
	2, 


*************************************************************************************************************************************************************************************/
int CEGDDMApp::ParseLine(char* pszNextLine) 	//parse line
{
	char		szNextLine[260] ;
	char		*token ;
	char		szCommand[256] ;
	char		szValue[256] ;

	try
	{

		sprintf( szNextLine, pszNextLine) ;		//copy input to a local string.

		if( strlen(szNextLine) < 2 )
		{
			return -1;
		}

		//check if it is a comment
		if( (*szNextLine == '/') && (*(szNextLine + 1) == '/'))	
		{
			//this is a comment, so ignore and retrun 1
			return 1 ;
		}
		
		token = strtok( szNextLine, "=\r\n") ;	//find command neame
		if( token == NULL)
			return 1 ;

		sprintf( szCommand, token) ;		//copy token to command

		token = strtok( NULL, "\r\n//;") ;

		if( token != NULL)
		{
			sprintf( szValue, token) ;		//copy value from token
			if( strlen( szValue) <= 0 || strlen(szCommand) <= 0)
			{
				return -1 ;
			}

			ExecCommand( szCommand, szValue) ;
		}
		else
		{
			ExecCommand( szCommand, "11") ;
		}

		return 2 ;
	}
	catch(...)
	{
		sprintf(m_szError,"Unhandeled Exception in ParseLine Function");
		WriteLog(m_szError);
		return 1;
	}

}





/***************************************************************************************************************
Proc Name	: ExecCommand
Purpose		: This function is used in executing a command, second argument may of may not be used.
Processing  : Check command one by one, and call corrosponding function.			
*************************************************************************************************************/
bool	CEGDDMApp::ExecCommand(char*  pszCommand,char* pszValue) 
{

	char		szLog[512] ;
	
	try
	{
		//check if it is first command.
		if( strcmp( pszCommand, m_CommandInfo[0].szCommand) == 0)
		{
			SetCurrentPath(pszValue) ;	//set current path 
			sprintf(szLog,"SetPath = %s",pszValue);
			WriteLog(szLog);
			return true ;
		}


		if( strcmp( pszCommand, m_CommandInfo[1].szCommand) == 0)
		{
			SetPollingTime(pszValue) ;	//set polling time.
			sprintf(szLog,"SetPollingTime = %s",pszValue);
			WriteLog(szLog);
			return true ;
		}

		if( strcmp( pszCommand, m_CommandInfo[2].szCommand) == 0)
		{
			SetDeleteDaysBefore(pszValue) ;	//set del hours before.
			sprintf(szLog,"SetDeleteDaysBefore = %s",pszValue);
			WriteLog(szLog);
			return true ;
		}

		if( strcmp( pszCommand, m_CommandInfo[3].szCommand) == 0)
		{
			SetFileExtention(pszValue) ;	//set del file extention.
			sprintf(szLog,"SetFileExtension = %s",pszValue);
			WriteLog(szLog);
			return true ;
		}

		if( strcmp( pszCommand, m_CommandInfo[4].szCommand) == 0)
		{
			DeleteFileBefore( ) ;				//delete all files of an extention, and before some date
			return true ;
		}

		if( strcmp( pszCommand, m_CommandInfo[5].szCommand) == 0)
		{
			SetLogFileName(pszValue) ;				//delete all files of an extention, and before some date
			sprintf(szLog,"LogFileNAme=%s",pszValue );
			WriteLog(szLog);
			return true ;
		}

		if( strcmp( pszCommand, m_CommandInfo[6].szCommand) == 0)
		{
			SetDestinationPath(pszValue) ;		//setting the destination path	
			sprintf(szLog,"SetDestinationPath = %s",pszValue);
			WriteLog(szLog);
			return true ;
		}

		if( strcmp( pszCommand, m_CommandInfo[7].szCommand) == 0)
		{
			CopyFileBefore() ;		//setting the destination path	
			return true ;
		}

		if( strcmp( pszCommand, m_CommandInfo[8].szCommand) == 0)
		{
			SetRecursionState(pszValue);	//setting the recursion type
			sprintf(szLog,"SetRecursionValue = %s",pszValue);
			WriteLog(szLog);
			return true ;
		}
		
		return false;
	}
	catch(...)
	{
		sprintf(m_szError,"Unhandeled Exception in ExecCommand");
		WriteLog(m_szError);
		return true;
	}

}





/*bool CEGDDMApp::SetRecursionState(char *pszValue)
{
	char		szCurPath[MAX_PATH];
	char		szFileName[MAX_PATH];
	long		fHandle;
	_finddata_t		finfo;

	try
	{
		m_bRecursion = atoi(pszValue);

		return true ;
	}
	catch(...)
	{
		sprintf(m_szError,"Unhandeled Exception in SetRecursionState");
		WriteLog(m_szError);
		return true;

	}





	/*sprintf(szFileName,"%s\\%s",m_szCurPath,m_szCurExtention);
	do
	{
		fHandle = _findfirst( szFileName, &finfo) ;		//find first file

		if( fHandle == -1)
		{
			return false;		//return successfully, becoz no such file exist
		}

		//sprintf(szFileName,"%s\\%s",szFileName,finfo.name);//adding file name to the current path

		if(finfo.attrib & _A_SUBDIR)//check if it is a subdir
		{
			if( m_bRecursion == 1 )
			{
				if( szFileName != "." && szFileName != ".." && strlen(szFileName)>=3)
				{
					SetRecursionState(szFileName);
				}
			}
		}

		return true;
	}while(_findnext(fHandle,&finfo) == 0);
	_findclose(fHandle);
	return  true;


		

}*/

	

/*****************************************************************************************************************************************
Proc Name :	SetFileExtention
Processing: Sets delete file extention.
********************************************************************************************************************************************/
bool CEGDDMApp::SetFileExtention( char* pszExtention)
{
	try
	{

		if( strlen(pszExtention) > MAX_EXTENTION_LEN )
		{
			m_bErrorFlag = true ;
			sprintf( m_szError, "File extention was not correct, more than maximum characters") ;
			WriteLog(m_szError) ;
			return false ;
		}
		sprintf( m_szCurExtention, pszExtention) ;

		return true ;
	}
	catch(...)
	{
		sprintf(m_szError,"Unhandeled Exception in SetFileExtention function");
		WriteLog(m_szError);
		return true;
	}
	
}







/******************************************************************************************************************************************
Proc Name : SetDestinationPath
Purpose : Sets destination path to copy yhe folders.
Procedure:
Check is passed argument(destination path) is null,if yes,return
otherwise
assign destination path to the member variable
***********************************************************************************************************************************************/


void	CEGDDMApp::SetDestinationPath( char* pszPath) 		//set current path 
{

	try
	{
		

		if( pszPath == NULL)	//check input path should not be NULL
		{
			m_bErrorFlag = true ;
			sprintf( m_szError, "Supplied Input path is not a valid path");
			return ;
		}
	
		sprintf( m_szDestPath, pszPath) ;	//copy destination path to member var
	}
	catch(...)
	{
		sprintf(m_szError,"Unhandeled Exception in SetDestinationPath function");
		WriteLog(m_szError);
		return ;
	}

	

}













/**************************************************************************************************************************************************************************
Proc Name: CopyFileBefore
Purpose:	Delete all files in the m_szCurPath which have
				1. having extention m_szCurExtention, 
				2. have creation time less than current time - m_lDeleteHourBefore 
Processing
	1. Enumerate all files in this folder having current extention.
	2. Check whether it's last access time is less than current time - hours.
	
time64 returns the time elapsed in seconds

*********************************************************************************************************************************************************************************/



void	CEGDDMApp::CopyFileBefore()
{

	try
	{

		m_bIsDeleteSet = false ;
		ProcessFileBefore(m_szCurPath, m_szDestPath);

		return ;
	}
	catch(...)
	{
		sprintf(m_szError,"Unhandeled Exception in SetRecursionState");
		WriteLog(m_szError);
		return ;

	}



	/*_finddata_t				finfo ;
	__time64_t				now ;		//current time (sec elapsed after 1 jan, 1970
	char					szFileName[260] ;
	long					lSecDelBefore ;		//seconds converted from m_lDeleteHourBefore ;
	long					fHandle ;
	int						nLenExt ;
	int						nIndexFileName ;
	char					szSourceFileName[MAX_PATH];
	
	
	try
	{

			sprintf(szFileName, "%s\\%s", m_szCurPath, m_szCurExtention) ;
			_time64(&now) ;						//get current time.

			lSecDelBefore = 3600 * 24 * m_lDeleteDaysBefore ;	//convert hours to seconds.

			fHandle = _findfirst( szFileName, &finfo) ;		//find first file
			if( fHandle == -1)// && _A_SUBDIR)	
			{
				return ;		//return successfully, becoz no such file exist
			}
			


			nLenExt = strlen( m_szCurExtention) ;

			do	//repeat for all files
			{

				//this code is commented, and can be included if we want to check extention of each file.
				//check whether the file contains extention 
				nIndexFileName = strlen( finfo.name ) ; //get len of file.
				if( nIndexFileName < nLenExt)		//extention should be small always.
					continue ;

				for ( int i = nLenExt ; i > 0; i--)	//check if extention does not match then continue.
				{
					if( i == nLenExt - 2)	//extentin can be of three chars only
						break ;
					if( finfo.name[nIndexFileName - 1] != m_szCurExtention[ i - 1])
					{
						return ;
						//continue ;
					}

					nIndexFileName -- ;
				}
		  


				if( finfo.time_access < now - lSecDelBefore )	//if this file is older than lSecDelBefore
				{
					//setting the source file name	
					sprintf(szSourceFileName,"%s\\%s",m_szCurPath,finfo.name);
					
					CopyFolder(szSourceFileName, m_szDestPath);


				}


			} while( _findnext( fHandle, &finfo) == 0) ;		//find next file


			return ;

	
	}
	catch(...)
	{
		sprintf(m_szError,"Unhandeled Exception in CopyFileBefore function");
		WriteLog(m_szError);
		return ;
	}*/

}






bool CEGDDMApp::CopyFolder(char* pstrSource, char* pstrDest)
{
	//Check whether there are any files in the recording directory
	_finddata_t fInfo;
	long fHandle;
	
	char		szTemp[MAX_PATH],szSource1[MAX_PATH];
	char		szDest1[MAX_PATH],szSourceFile[MAX_PATH],szDestFile[MAX_PATH];


	try
	{
		sprintf(szTemp,"%s",pstrSource);
		
		if (( fHandle = _findfirst(szTemp, &fInfo)) == -1)
		{
			return TRUE;
		}
		else
		{
			do
			{
				if (fInfo.attrib & _A_SUBDIR)
				{
					//sprintf(strTemp,"%s",fInfo.name);
					sprintf(szTemp,"%s",fInfo.name) ;
					if( szTemp != "." && szTemp != ".." && strlen(szTemp)>=3)
					{
						sprintf(szSource1,"%s\\%s",m_szCurPath,fInfo.name);
						sprintf(szDest1,"%s\\%s",m_szDestPath,fInfo.name);
						CopyFolder(szSource1,szDest1);
					}
				}
				else
				{
					sprintf(szSourceFile,"%s\\%s",m_szCurPath,fInfo.name);
					sprintf(szDestFile,"%s\\%s",m_szDestPath,fInfo.name);
					CopyFile(szSourceFile,szDestFile,false);
				}
			}while(_findnext(fHandle,&fInfo) == 0);
		}	
		_findclose(fHandle);
		return TRUE;
	}
	catch(...)
	{
		sprintf(m_szError,"Unhandeled Exception in CopyFolder function");
		WriteLog(m_szError);
		return true;
	}
}








void	CEGDDMApp::SetRecursionState(char	*pszRecState)
{
	try
	{
		if(pszRecState == NULL)
		{
			sprintf( m_szError, "[ERROR] State of recursion is NULL, contains previous value") ;
			WriteLog(m_szError);
			return ;
		}


		m_bRecursion  =  atoi(pszRecState) == 0 ? false: true;
	}
	catch(...)
	{
	
		sprintf(m_szError,"Unhandeled Exception in SetRecursionState function");
		WriteLog(m_szError);
		return ;
	
	}
	
}






/**************************************************************************************************************************************************************************
Proc Name: DeleteFileBefore
Purpose:	Delete all files in the m_szCurPath which have
				1. having extention m_szCurExtention, 
				2. have creation time less than current time - m_lDeleteHourBefore 
Processing
	1. Enumerate all files in this folder having current extention.
	2. Check whether it's last access time is less than current time - hours.
	
time64 returns the time elapsed in seconds

*********************************************************************************************************************************************************************************/
void	CEGDDMApp::ProcessFileBefore(char* pSource, char* pDest)
{
	_finddata_t				finfo ;
	time_t				now ;		//current time (sec elapsed after 1 jan, 1970
	//char					szFileName[260] ;
	long					lSecDelBefore ;		//seconds converted from m_lDeleteHourBefore ;
	long					fHandle ;
	int						nLenExt ;
	int						nIndexFileName ;
	int						nSourceLen;
	int						nDestLen;
	char					szTempDestPath[MAX_PATH];
	int i;
		
	char					szCurFileName[MAX_PATH];
	char					szCurTempFileName[MAX_PATH];
	char					szCurPath[MAX_PATH];
	char					szDestFileName[MAX_PATH];
	bool					bIsNotFound = true ;
	try
	{

		now = time(NULL) ;						//get current time.
		lSecDelBefore = 3600 * 24 * m_lDeleteDaysBefore ;	//convert hours to seconds.

		sprintf( szCurPath,"%s\\*", pSource) ;				
		fHandle = _findfirst( szCurPath , &finfo) ;		//find first file
		if( fHandle == -1)	
		{
			return ;		//return successfully, becoz no such file exist
		}
	
		nLenExt = strlen( m_szCurExtention) ;

		do	//repeat for all files
		{

			sprintf( szCurFileName, finfo.name) ;
			
			if( (strcmp(szCurFileName, ".") == 0 ) || (strcmp(szCurFileName, "..") == 0 ))
				continue ;

			if( (finfo.attrib & _A_SUBDIR) && (m_bRecursion == true))	//if it is directory and recursion is true
			{
				sprintf(szCurTempFileName, "%s\\%s", pSource ,finfo.name) ;
				sprintf( szDestFileName, "%s\\%s",pDest, finfo.name) ;

				if( m_bIsDeleteSet == false)
					CreateDirectory( szDestFileName, NULL) ;
				ProcessFileBefore(szCurTempFileName , szDestFileName) ;

				
				continue ;
			}


			nIndexFileName = strlen( finfo.name ) ; //get len of file.
			if( nIndexFileName < nLenExt)		//extention should be small always.
				continue ;

			
			
			

			bIsNotFound = false ;
			for ( int i = nLenExt ; i > 0; i--)	//check if extention does not match then continue.
			{
			//	if( i == nLenExt - 2)	//extentin can be of three chars only
			//		break ;
				if( finfo.name[nIndexFileName - 1] != m_szCurExtention[ i - 1])
				{
					bIsNotFound = true ;
					//return ;
					//continue ;
				}

				nIndexFileName -- ;
			}
			

			if( bIsNotFound == true )	//file not found
				continue ;
			

			if( finfo.time_access < now - lSecDelBefore )	//if this file is older than lSecDelBefore
			{
				sprintf( szCurFileName, "%s\\%s",pSource, finfo.name) ;


				if( m_bIsDeleteSet == true)
					_unlink(szCurFileName) ;//delete this file
				else
				{
					if(m_bRecursion == true)
					{
					//check if destination path is in source path 
				
						nSourceLen = strlen(m_szCurPath);//source path len
						nDestLen = strlen(m_szDestPath);//destpath lem
			
						if((nSourceLen == nDestLen) || (nSourceLen < nDestLen))//checking the length of thge paths
						{
							for(i=0 ; i<nSourceLen ; i++)
								szTempDestPath[i] = m_szDestPath[i];
							
							szTempDestPath[i] = '\0';
							if(strcmp(szTempDestPath,m_szCurPath) == 0)
							{
								m_bErrorFlag = true;
								sprintf(m_szError,"Destination Path is in the source path which is not possible");
								WriteLog(m_szError);
								printf("\nDestination Path %s is in Source path %s which is not possible\n",m_szDestPath,m_szCurPath);
								break;
							}
						}

						sprintf( szDestFileName, "%s\\%s",pDest, finfo.name) ;
						CopyFile( szCurFileName, szDestFileName, false) ;
						
					}
					else
					{
						sprintf( szDestFileName, "%s\\%s",pDest, finfo.name) ;
						CopyFile( szCurFileName, szDestFileName, false) ;
					}
				}

							
			}
			


		} while( _findnext( fHandle, &finfo) == 0) ;		//find next file

		return ;
	}
	catch(...)
	{
		sprintf(m_szError , "Unhandeled Exception in CopyFileBeforeFunction");
		WriteLog(m_szError);
		return;
	}

}


/*
*
*/
#undef __FNNAME__
#define __FNNAME__ "startCleanupDaemon"
void egStartCleanupDaemon(char *pszCfgFileName)
{
	_beginthread(cleanupDaemon, 0, (void *)pszCfgFileName);

	return;
}


/*
*
*/
void cleanupDaemon(void *p)
{
	char *pszFileName = (char *)p;

	CEGDDMApp	Q;


	Q.InitCEGDDMApp(pszFileName) ;

	
	return;
}
