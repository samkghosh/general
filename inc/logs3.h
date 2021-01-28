#ifndef __LOGS_H__
#define __LOGS_H__


//X: Level
// T : thread or run number 
//Y: Message
//Z: Display or not

#ifndef __McDBGLOGOBJECT__
#define __McDBGLOGOBJECT__	dbgLog
#endif


// McDbgLog(Level, Message, display =true/false) 
//#define McDbgLog(X,Y,Z)	dbgLog.writeLog(X, Y, Z, __LINE__, __FNNAME__); 
#define McDbgLog(L,M)	__McDBGLOGOBJECT__.writeLog(L, M, __LINE__, __FNNAME__); 


// McDbgLog(Level, Threadnumber. Message, display =true/false) 
#define McDbgLog1(L,T,M)	__McDBGLOGOBJECT__.writeLog(L, T, M, __LINE__, __FNNAME__); 




// McDbgLog(Level, Threadnumber. Message, display =true/false) 
//#define McDbgLog1(X,T,Y,Z)	dbgLog.writeLog(X, T, Y, Z, __LINE__, __FNNAME__); 



enum
{
	LOG_NODISP =0,
	LOG_DISP
};



#define LOGLEVEL0		0x01
#define LOGLEVEL1		0x02
#define LOGLEVEL2		0x04
#define LOGLEVEL3		0x08
#define LOGLEVEL4		0x10
#define LOGLEVEL5		0x20
#define LOGLEVEL6		0x40
#define LOGLEVEL7		0x80
#define LOGLEVELALL		0xFF


#define MAXMSGSZ 2048

typedef struct __LogInfo
{
	int iLevel;
	int iProc;
	char szMsg[MAXMSGSZ];
	int iLine;
	char szFuncName[64];
	//CRITICAL_SECTION      csLog;
	int iDynamic;
	char szPath[64];
	char szPrefix[16];
	struct __LogInfo *next;
	time_t ulLogTime;
}T_LogInfo;




class Logs  
{
private:
	char	logPath[256];	// log file name
	int		maxLogLevel;
	int		dispLogLevel;
	char	preFix[10];
	bool	dynamic;

//	int appendText(char *, char *);
	int writeFile(char *, char *);
	//CRITICAL_SECTION      csLog;

public:
	Logs();
	Logs(char *prefix, bool change = false, int logLevel = 0);
	void initLogs(char * path, char *prefix, int change, int logLevel);

	virtual	void displayLine(char *);
	virtual ~Logs();
	void writeLog(int level, char * msg, int line = 0, char * funcName = '\0');
	void writeLog(int level, int thread, char * msg, int line = 0, char * funcName = '\0');
	void setLogLevel(int level);
//	void setLogFileName(char *prefix, bool change = false);
	void setPrefix(char *prefix, bool change=false);
	void setDispLogLevel(int level);
//	void unSetLogLevel(int level);
	void unSetDispLogLevel(int level);



};



#define LOG_EVT_ERROR		0
#define LOG_EVT_WARNING		1
#define LOG_EVT_INFO		2

int writeToEventViewer(char * appName, char *szMsg, int type);

#endif
