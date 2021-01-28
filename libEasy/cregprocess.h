
#define	SUCCESS		0
#define	FAIL		-1
#define NOTAKEY		-2



class CRegProcess
{

public:

//	CRegProcess();

	// bool flag is added to give any user to read the values using the app
	// if the flag is true, then the user should be Administrator for modifying Reg entry
	CRegProcess(char* appName, bool bWrite=false);
	~CRegProcess();

	
	
	int CreateSectionKey(char* newSection);

	
	int DeleteSectionKey(char* section);

	
	int CreateSubKey(char* section,char* newkey,char* value=NULL);

	
	int DeleteSubKey(char* section,char* key);

	
	int SetKeyValue(char* section,char* key,char* value=NULL);
	int SetKeyValue(char* section,char* key,unsigned long value);

	int GetKeyValue(char* section,char* key,char* value);
	int GetKeyValue(char* section,char* key,unsigned long * value);
	bool isOk();

private:
	

	char aName[128];

//	HKEY appKey;
	

	//Genuine Private Functions
	int CreateAppKey(char* appName);
	
/*	int OpenKey(HKEY pKey,char* appName,HKEY* newHKey);

	int SetkValue(HKEY secKey,char* key,char* value);

	int SetkValue(HKEY secKey,char* key,unsigned long value);
	
	int GetkValue(HKEY secKey,char* key,char* value);

	int GetkValue(HKEY secKey,char* key,unsigned long * value);

*/	

	



};

