#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__


//Timeout constants
#define			SP_READ_INTERVAL_TIMEOUT		0
#define			SP_READ_TIMEOUT_MULTIPLIER		0
#define			SP_READ_TIMEOUT_CONSTANT		0
#define			SP_WRITE_TIMEOUT_MULTIPLIER		0
#define			SP_WRITE_TIMEOUT_CONSTANT		0


//Retry Count
#define			SP_MAXTRY						1


#define			SP_PARITY_NONE					0
#define			SP_PARITY_ODD					1
#define			SP_PARITY_EVEN					2

#define			SP_STOPBIT_ONE					1
#define			SP_STOPBIT_ONEPOINTFIVE			2
#define			SP_STOPBIT_TWO					3



//return values of connect function
#define			SP_CONNECT_NOERROR				0x00
#define			SP_CONNECT_ERROR_SET_MASK		0x01
#define			SP_CONNECT_ERROR_SET_TIMEOUTS	0x02
#define			SP_CONNECT_ERROR_SET_STATE		0x04
#define			SP_CONNECT_ERROR_HANDLE			0x08
#define			SP_CONNECT_ERROR_PORT_ARG		0x10





/*
* Class Definition
*/

class SerialPort
{

public:
	static int	iObjectCount;	//to keep track of the number of objects of this class


private:

	char              port[10];				         // port name "COM1",...
    int               rate;                          // baudrate
	int			      parityMode;					 // parity (odd/even/none)
    HANDLE            serial_handle;                 // ...

	int	readIntervalTimeOut;	//read time out between tho successive characters					
	int	readTimeOutMultiplier;	//no. of bytes into multiplier
	int	readTimeOutConstant;	//constant to be added

	int writeTimeOutMultiplier; //no. of bytes into multiplier
	int writeTimeOutConstant;	//constant to be added

	int			MaxTry_Count;			//max tries for send/recieve
	HANDLE		hMutex;			//Mutext to serialize requests

public:
	bool				bConnect;
	HWND				m_hLogs;
   
public:
    SerialPort();		//contructor
    ~SerialPort();		//destructor

	
	////////////////////////////////////////////////////////////////////////////////
	//connection related functions 

	//to connect to serial port
    //int connect(char *portname,	int baudrate,int parity);
	int connect(char * szPort,int nBaud,int nParity,int nDataBits,int nStopBit);

	//to disconnect from serial port
    void disconnect();

	/////////////////////////////////////////////////////////////////////////////////
	//property seting functions 

	//to set read timeouts
	void setReadTimeOuts(int readIntervalTimeOut,int readTimeOutMultiplier = 0,int readTimeOutConstant = 0);

	//to set write time outs
	void setWriteTimeOuts(int writeTimeOutMultiplier=0,int writeTimeOutConstant=0);

	//to set max try count
	void setMaxTryCount(int count);

	//Generic Functions
	int sendArray(char *buffer, int len);
    int getArray(char *buffer, int len);

};



#endif __SERIALPORT_H__