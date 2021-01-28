
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the EGLM_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// EGLM_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef WIN32
#ifdef EGLM_EXPORTS
#define EGLM_API __declspec(dllexport)
#else
#define EGLM_API __declspec(dllimport)
#endif
#else
#define EGLM_API
#endif

#define __EGLMSDK_H__
#define EGLM_NOLOCATION			0x01
#define EGLM_NOACCESS			0x02
#define EGLM_NOENTRYFOUND		0x03
#define EGLM_OPENERROR			0x04
#define	EGLM_TIMEEXPIRED		0x05
#define EGLM_OK					0x00
#define EGLM_INVALID			0x06

// This class is exported from the egLm.dll
class EGLM_API CEgLm {
	char productId[32];
public:
	CEgLm(void);
	CEgLm(char *pId);

	int check();
	int getToken(char*tokenName, char * tokenValue);
	char * getVersion();
	int getCount();

	// TODO: add your methods here.
};

extern EGLM_API int nEgLm;

EGLM_API int fnEgLm(void);

