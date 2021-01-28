/*****************************************************************************************************************
		File Name	:	cleanupFile.h
		Purpose		:	It defines all macro used in the project.

***********************************************************************************************************************/
#ifndef __CLEANUPFILE_H__
#define __CLEANUPFILE_H__


#define			DEFAULT_POLLTIME_HOURS			24
#define			DEFAULT_CURRENTPATH				".\\"
#define			DEFAULT_DELETEBEFOREDAYS		30	//30 days


#define			MAX_COMMAND						32 
#define			MAX_COMMANDS_NO					10
#define			MAX_COMMANDVALUE				256
#define			MAX_COMMANDPROC					32
#define			MAX_EXTENTION_LEN				5

#define			ITEM_SETCURPATH					1
#define			ITEM_SETDELBEFOREDAYS			2
#define			ITEM_DELFILEEXT					3

#define			MAX_DELDAYS_LIMIT				200
#define			MIN_DELDAYS_LIMIT				-1

typedef struct
{
	char	szCommand [MAX_COMMAND] ;
	//char	szCommandProc [MAX_COMMANDPROC] ;
	//bool	(*ptr) (char*) ;
	//int		nCommandIndex ;
}T_CommandInfo ;





class CEGDDMApp
{
public:
	CEGDDMApp(void);
	~CEGDDMApp(void);
public:
	bool			m_bErrorFlag ;		//it will be set when any error occures.
	char			m_szError[512] ;			//error description

private :
	char			m_szInputFileName[260] ;		//Name of the input script file, which contains all info.
	char			m_szCurPath[260] ;				//Path where it will search for files to delete.
	char			m_szDestPath[260];
	long			m_lDeleteDaysBefore ;				//All files will be deleted before these hours.
	char			m_szCurExtention[10] ;					//Current extention.
	long			m_lPollHour	;						//hour after which it will check again.
	bool			m_bRecursion;				//Recursion for deleting & moving the files.
	bool			m_bIsDeleteSet ;			//If true then delete,else copy
	bool			m_bToTerminate ;
	char			m_szLogFileName[260];		//to retrieve the log file name

public:
	void			SetCurrentPath( char* pszPath) ;					//set current path 
	void			SetPollingTime( char* pszHours)	;				//set polling time in hours.
	void			SetDestinationPath( char* pszPath);				//set destination path
	bool			CopyFolder( char* strSource, char* strDest) ;//to copy folders from source to destination
	void			CopyFileBefore();		//copy files from source to destination
	bool			InitCEGDDMApp(char* pszInputFileName) ;
	void			ExitCEGDDMApp() ;
    void			DeleteFileBefore() ;
	int				ParseLine(char* pszNextLine) ;	//parse line
	bool			ExecCommand( char* pszCommand,char*  pszValue) ;
	bool			SetDeleteDaysBefore( char* pszDays) ;
	bool			SetFileExtention( char* pszExtention) ;
	int				SetLogFileName(char* pszLogFileName);
	void			SetRecursionState(char	*pszRecState);
	void			ProcInterpreter () ;
	bool			WriteLog(char* pszLog) ;
	void			ProcessFileBefore(char* pSource, char* pDest) ;

	T_CommandInfo	m_CommandInfo[MAX_COMMANDS_NO] ;//structure to store the commands of script file.
};


void cleanupDaemon(void *p);


#endif
