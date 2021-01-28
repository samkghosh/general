//NFRserial.cpp

#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <time.h>

#include "SerialPort.h"


int SerialPort::iObjectCount = 0;


#define SP_MUTEX_NAME	"__SerialCommunicationPort__"
/*******************************************************************************
* NFRserial()
*
* Constructor
********************************************************************************/
SerialPort::SerialPort()
{
	char *szMutexName;

	++iObjectCount;		//Another object created

	parityMode       = SP_PARITY_NONE;
    port[0]          = 0;
    rate             = 0;
    serial_handle    = INVALID_HANDLE_VALUE;

	readIntervalTimeOut=SP_READ_INTERVAL_TIMEOUT;
	readTimeOutMultiplier=SP_READ_TIMEOUT_MULTIPLIER;
	readTimeOutConstant=SP_READ_TIMEOUT_CONSTANT;

	writeTimeOutMultiplier=SP_WRITE_TIMEOUT_MULTIPLIER;
	writeTimeOutConstant=SP_WRITE_TIMEOUT_CONSTANT;

	MaxTry_Count=SP_MAXTRY;

	bConnect=false;
	m_hLogs = NULL;
//	m_lTimeOutsec = 0;

	szMutexName = SP_MUTEX_NAME;

	/* create the mutex now */
	hMutex = CreateMutex( NULL, FALSE, szMutexName);
}


/*******************************************************************************
* ~SerialPort()
*
* Destructor
********************************************************************************/
SerialPort::~SerialPort()
{
    if (serial_handle!=INVALID_HANDLE_VALUE)
        CloseHandle(serial_handle);
    serial_handle = INVALID_HANDLE_VALUE;

	if(hMutex)
		CloseHandle(hMutex);
}




/*******************************************************************************
* setReadTimeOuts()
*
* Purpose : for setting read timeout values
*
* Parameters :readIntervalTimeOut
*			  readTimeOutMultiplier
*			  readTimeOutConstant	
********************************************************************************/
void SerialPort::setReadTimeOuts(int rdIntervalTmOut,int rdTmOutMultiplier,int rdTmOutConstant)
{
	readIntervalTimeOut=rdIntervalTmOut;
	readTimeOutMultiplier=rdTmOutMultiplier;
	readTimeOutConstant=rdTmOutConstant;
}


/*******************************************************************************
* setWriteTimeOuts()
*
* Purpose : for setting write timeout values
*
* Parameters :writeTimeOutMultiplier
*			  writeTimeOutConstant	
********************************************************************************/
void SerialPort::setWriteTimeOuts(int wrTimeOutMultiplier,int wrTimeOutConstant)
{
	writeTimeOutMultiplier=wrTimeOutMultiplier;
	writeTimeOutConstant=wrTimeOutConstant;
}


/*******************************************************************************
* setMaxTryCount()
*
* Purpose : For setting retry count value
*
* Parameters :retrycount
********************************************************************************/
void SerialPort::setMaxTryCount(int retrycount)
{
	MaxTry_Count=retrycount;
}


/*******************************************************************************
* disconnect()
*
* Purpose : To close connection with serial port
********************************************************************************/
void SerialPort::disconnect()
{
    if (serial_handle!=INVALID_HANDLE_VALUE)
        CloseHandle(serial_handle);
    serial_handle = INVALID_HANDLE_VALUE;
}


/*******************************************************************************
* connect()
*
* Purpose    :To connect to the serial port
*
* Parameters :port_arg		("COM1","COM2"......)
*			  rate_arg		(9600,19200,........)
*			  parity_arg	(spNONE,spODD,spEVEN)
*
* Ret Val    :SP_CONNECT_NOERROR	
*			  SP_CONNECT_ERROR_SET_MASK
*			  SP_CONNECT_ERROR_SET_TIMEOUTS
*			  SP_CONNECT_ERROR_SET_STATE
*			  SP_CONNECT_ERROR_HANDLE
*			  SP_CONNECT_ERROR_PORT_ARG
********************************************************************************/
int  SerialPort::connect(char *port_arg, int rate_arg, int parity_arg,int nDataBits,int nStopBit)
{
    int error;
    DCB  dcb;
    COMMTIMEOUTS cto = { readIntervalTimeOut,
						 readTimeOutMultiplier,
						 readTimeOutConstant,
						 writeTimeOutMultiplier,
						 writeTimeOutConstant };

    if (serial_handle!=INVALID_HANDLE_VALUE)
        CloseHandle(serial_handle);
    serial_handle = INVALID_HANDLE_VALUE;

    error = SP_CONNECT_NOERROR;

    if (port_arg!=0)
    {
        strncpy(port, port_arg, 10);
        rate      = rate_arg;
        parityMode= parity_arg;
        memset(&dcb,0,sizeof(dcb));

        // set DCB to configure the serial port
        dcb.DCBlength       = sizeof(dcb);                   
        
        //Configuring Serial Port 
        dcb.BaudRate        = rate;

        switch(parityMode)
        {
            case SP_PARITY_NONE:
                            dcb.Parity      = NOPARITY;
                            dcb.fParity     = 0;
                            break;
            case SP_PARITY_EVEN:
                            dcb.Parity      = EVENPARITY;
                            dcb.fParity     = 1;
                            break;
            case SP_PARITY_ODD:
                            dcb.Parity      = ODDPARITY;
                            dcb.fParity     = 1;
                            break;
        }

		switch(nStopBit)
		{
			case SP_STOPBIT_ONE:
							dcb.StopBits	= ONESTOPBIT;
							break;
			case SP_STOPBIT_ONEPOINTFIVE:
							dcb.StopBits	= ONE5STOPBITS;
							break;
			case SP_STOPBIT_TWO:
							dcb.StopBits	= TWOSTOPBITS;
							break;
		}

       // dcb.StopBits        = ONESTOPBIT;
        dcb.ByteSize        = nDataBits;
        
        dcb.fOutxCtsFlow    = 0;
        dcb.fOutxDsrFlow    = 0;
        dcb.fDtrControl     = DTR_CONTROL_DISABLE;
        dcb.fDsrSensitivity = 0;
        dcb.fRtsControl     = RTS_CONTROL_DISABLE;
        dcb.fOutX           = 0;
        dcb.fInX            = 0;
        
        dcb.fErrorChar      = 0;
        dcb.fBinary         = 1;
        dcb.fNull           = 0;
        dcb.fAbortOnError   = 0;
        dcb.wReserved       = 0;
        dcb.XonLim          = 2;
        dcb.XoffLim         = 4;
        dcb.XonChar         = 0x13;
        dcb.XoffChar        = 0x19;
        dcb.EvtChar         = 0;
        
        // opening serial port
		serial_handle    = CreateFile(port, GENERIC_READ | GENERIC_WRITE,
                               0, NULL, OPEN_EXISTING,NULL,NULL);

        if (serial_handle != INVALID_HANDLE_VALUE)
        {
            if(!SetCommMask(serial_handle, 0))
                error=SP_CONNECT_ERROR_SET_MASK;

            // set timeouts
            if(!SetCommTimeouts(serial_handle,&cto))
                error=SP_CONNECT_ERROR_SET_TIMEOUTS;

            // set DCB
            if(!SetCommState(serial_handle,&dcb))
                error=SP_CONNECT_ERROR_SET_STATE;
        }
        else
            error=SP_CONNECT_ERROR_HANDLE;
    }
    else
        error = SP_CONNECT_ERROR_PORT_ARG;


    if (error!=SP_CONNECT_NOERROR)
    {
        CloseHandle(serial_handle);
        serial_handle = INVALID_HANDLE_VALUE;
    }
    return(error);
}


/*******************************************************************************
* sendArray()
*
* Purpose    :To send a buffer over serial port
*
* Parameters :buffer  (buffer to send)		
*			  length  (length of buffer)
*
* Ret Val    :Number of bytes send
********************************************************************************/
int SerialPort::sendArray(char *buffer, int length)
{
    unsigned long result;

    if (serial_handle!=INVALID_HANDLE_VALUE)
        WriteFile(serial_handle, buffer, length, &result, NULL);

	return ((int) result);
}


/*******************************************************************************
* getArray()
*
* Purpose    :To recieve a buffer over serial port
*
* Parameters :buffer  (buffer to recieve)		
*			  length  (length of buffer)
*
* Ret Val    :Number of bytes recieved
********************************************************************************/
int SerialPort::getArray(char *buffer, int length)
{
   unsigned long read_nbr;

    read_nbr = 0;
    if (serial_handle!=INVALID_HANDLE_VALUE)
        ReadFile(serial_handle, buffer, length, &read_nbr, NULL);

    return((int) read_nbr);
}

