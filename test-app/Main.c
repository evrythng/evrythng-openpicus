/** @mainpage OpenPicus
*
*  
<B>OpenPicus</B> is open source Hardware and Software wireless project to enable Smart Sensors and Internet of Things.
<UL>
	<LI><B>Hardware platform:</B> it's modular, the Modules are PICUSes while the Carrier Boards are their NESTs</LI>
	<LI><B>Wireless:</B> Wi-Fi or Bluetooth. You have full control of the Stack and power down modes.</LI>
	<LI><B>Software Framework:</B> your Apps can control the functions of the Protocol Stack, but you don't need to be an expert of it.</LI>
	<LI><B>Development tool:</B> free IDE is ready to let you start development immediately.</LI>
	<LI><B>Serial Bootloader:</B> Brutus loaded on modules, you don't need a programmer</LI>
</UL><BR><BR>
<B>Flyport</B> is the first OpenPicus device, embedding the TCP/IP and WiFi stack for wireless communication with 802.11 devices. 
The Flyport also includes a real time operating system (FreeRTOS - www.freertos.org) to manage easily the TCP/IP stack and the user application in two different tasks.
\image html images/flyport_top.jpg

\image latex images/flyport_top.jpg
* @authors Gabriele Allegria
* @authors Claudio Carnevali<BR>
Special thanks to Andrea Seraghiti for the support in development.
*/


/****************************************************************************
  SECTION 	Includes
****************************************************************************/
#define __MAIN_C
#include "taskTCPIP.h"
#include "taskFlyport.h"

#include "ARPlib.h"
/*****************************************************************************
 *								--- CONFIGURATION BITS ---					 *
 ****************************************************************************/
_CONFIG2(FNOSC_PRI & POSCMOD_HS & IOL1WAY_OFF )		// Primary HS OSC 
_CONFIG1(JTAGEN_OFF & FWDTEN_OFF & ICS_PGx2 )		// JTAG off, watchdog timer off
//_CONFIG3(WPDIS_WPDIS)
	
#include "TCPIP Stack/WF_Utils.h"
#include "TCPIP Stack/TCPIP_Utils.h"

#if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
#include "TCPIP Stack/ZeroconfLinkLocal.h"
#endif
#if defined(STACK_USE_ZEROCONF_MDNS_SD)
#include "TCPIP Stack/ZeroconfMulticastDNS.h"
#endif

//#define FW_VER_ON_U1

APP_CONFIG AppConfig;
APP_CONFIG NETConf[2];

UINT8 ConnectionProfileID = 0;

int _WFStat = NOT_CONNECTED;
int WFStatusold;
int WFConnection = WF_DEFAULT;

static int Cmd = 0;


//	FrontEnd variables
BYTE xIPAddress[100];
WORD xTCPPort = 22;
TCP_SOCKET xSocket = INVALID_SOCKET;
int xFrontEndStat = 0;
int xFrontEndStatRet = 0;
int xErr = 0;
BOOL xBool = FALSE;
WORD xWord;
char *xChar;
BYTE *xByte;
BYTE xByte2;
BYTE xByte3;
int xInt;
int xInt2;
NODE_INFO xNode;

BOOL WPAWrong = FALSE;
extern SMTP_POINTERS SMTPClient;
#if defined (FLYPORT_WF)
extern RSSI_VAL myRSSI;
	tWFNetwork xNet;
#endif

#if MAX_UDP_SOCKETS_FREERTOS>0
//UDP variables:
UDP_SOCKET udpSocket[MAX_UDP_SOCKETS_FREERTOS];
WORD udpRxLen[MAX_UDP_SOCKETS_FREERTOS];
WORD udpRxLenGlobal[MAX_UDP_SOCKETS_FREERTOS];
BYTE numUdpSocket = 0;
WORD BUFFER_UDP_LEN[MAX_UDP_SOCKETS_FREERTOS];
BYTE* udpBuffer[MAX_UDP_SOCKETS_FREERTOS];

BYTE udpBuffer1[BUFFER1_UDP_LEN];

#if MAX_UDP_SOCKETS_FREERTOS>1
BYTE udpBuffer2[BUFFER2_UDP_LEN];
#endif

#if MAX_UDP_SOCKETS_FREERTOS>2
BYTE udpBuffer3[BUFFER3_UDP_LEN];
#endif

#if MAX_UDP_SOCKETS_FREERTOS>3
BYTE udpBuffer4[BUFFER4_UDP_LEN];
#endif

UDP_PORT xUDPPort[MAX_UDP_SOCKETS_FREERTOS];

BYTE* p_udp_wifiram[MAX_UDP_SOCKETS_FREERTOS];		//internal use
BYTE* p_udp_data[MAX_UDP_SOCKETS_FREERTOS];			//user data
WORD tmp_len[MAX_UDP_SOCKETS_FREERTOS];
DWORD xUDPRemoteHost;
WORD xUDPLocalPort = 0;
WORD xUDPRemotePort = 0;
BYTE activeUdpSocket = 0;
BYTE callbackUdpSocket = 0;
int udpErr = 0;
BOOL udpBool = FALSE;
WORD udpWord;
int udpInt;
BYTE *udpByte;

int datadiscard = 0;
BOOL UDPoverflow = 0;
BOOL UDPoverflowFlag[MAX_UDP_SOCKETS_FREERTOS];
#endif

//	RTOS variables
xTaskHandle hTCPIPTask;
xTaskHandle hFlyTask;
xTaskHandle hTimerTask;
xQueueHandle xQueue;
xSemaphoreHandle xSemFrontEnd = NULL;
xSemaphoreHandle xSemHW = NULL;
portBASE_TYPE xStatus;

static int (*FP[40])();


void CmdCheck()
{
	#if MAX_UDP_SOCKETS_FREERTOS>0 //UDP Stack
	activeUdpSocket=0;
	while (activeUdpSocket < MAX_UDP_SOCKETS_FREERTOS) 
	{
		//Ring Buffer
		if(udpSocket[activeUdpSocket] != INVALID_UDP_SOCKET)
		{
			//reads udp data and adds in ring buffer
			udpRxLen[activeUdpSocket] = UDPIsGetReady(udpSocket[activeUdpSocket]);

			if(udpRxLen[activeUdpSocket] > (BUFFER_UDP_LEN[activeUdpSocket] - udpRxLenGlobal[activeUdpSocket]))
			{
				// Since there is not enough space to store the packet,
				// discard all the transceiver RAM content related to UDP
				datadiscard = udpRxLen[activeUdpSocket];
				
				// Set the Overflow Flags
				UDPoverflow = 1;
				UDPoverflowFlag[activeUdpSocket] = 1;
				
				// Now discard all remaining data in Udp Rx:
				// Create a dummy array
				BYTE removeArray[BUFFER_UDP_FIXED_LEN];
				// update the datadiscard value
				while(datadiscard > 0)
				{
					datadiscard -= UDPGetArray(removeArray, BUFFER_UDP_FIXED_LEN);
				}
			}
			
			if (udpRxLen[activeUdpSocket] > 0)
			{
				if( (p_udp_wifiram[activeUdpSocket]+udpRxLen[activeUdpSocket]) <= (udpBuffer[activeUdpSocket]+BUFFER_UDP_LEN[activeUdpSocket]) ) //append to buffer
				{
					UDPGetArray(p_udp_wifiram[activeUdpSocket], udpRxLen[activeUdpSocket]); //data
					p_udp_wifiram[activeUdpSocket] += udpRxLen[activeUdpSocket];
				}
				else //append to buffer near end, or add to buffer from start
				{
					tmp_len[activeUdpSocket] = ((udpBuffer[activeUdpSocket]+BUFFER_UDP_LEN[activeUdpSocket]) - p_udp_wifiram[activeUdpSocket]);  //free space on ring buffer
					if(tmp_len[activeUdpSocket] > 0) //fill buffer and add data from start
					{
						UDPGetArray(p_udp_wifiram[activeUdpSocket], tmp_len[activeUdpSocket]);
						p_udp_wifiram[activeUdpSocket] = udpBuffer[activeUdpSocket];
						UDPGetArray(p_udp_wifiram[activeUdpSocket], udpRxLen[activeUdpSocket] - tmp_len[activeUdpSocket] );
						p_udp_wifiram[activeUdpSocket] += (udpRxLen[activeUdpSocket] - tmp_len[activeUdpSocket]);
					}
					else //add data to buffer from start
					{ 
						p_udp_wifiram[activeUdpSocket] = udpBuffer[activeUdpSocket];
						UDPGetArray(p_udp_wifiram[activeUdpSocket], udpRxLen[activeUdpSocket]);
						p_udp_wifiram[activeUdpSocket] += udpRxLen[activeUdpSocket];
					}
				}
				udpRxLenGlobal[activeUdpSocket] += udpRxLen[activeUdpSocket];	
			
			} //end ring buffer
		}
		activeUdpSocket++;
	}
	#endif //UDP STACK

	if (Cmd != 0)
	{
		int fresult = 0;
		while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);
		if (xFrontEndStat == 1)
		{
			fresult = FP[Cmd]();
			xFrontEndStat = xFrontEndStatRet;
			xSemaphoreGive(xSemFrontEnd);
			Cmd = 0;
			taskYIELD();
		}
	}
}



#if defined( WF_CS_TRIS )
    /* used for WiFi assertions */
    #ifdef WF_DEBUG
        #define WF_MODULE_NUMBER   WF_MODULE_MAIN_DEMO
    #endif
#endif /* WF_CS_TRIS */


/****************************************************************************
  MAIN APPLICATION ENTRY POINT
****************************************************************************/
int main(void)

{	
	//	Queue creation - will be used for communication between the stack and other tasks
	xQueue = xQueueCreate(3, sizeof (int));

	xSemFrontEnd = xSemaphoreCreateMutex();

	
	// Initialize application specific hardware
	HWInit(HWDEFAULT);

	// Initializing the UART for the debug
	#if defined	(STACK_USE_UART)
	UARTInit(1, UART_DBG_DEF_BAUD);
	UARTOn(1);
	_dbgwrite("Flyport starting...");
	#endif	
	
	//	RTOS starting
	if (xSemFrontEnd != NULL) 
	{
		// Creates the task to handle all TCPIP functions
		xTaskCreate(TCPIPTask, (signed char*) "TCP", STACK_SIZE_TCPIP,
		NULL, tskIDLE_PRIORITY + 1, &hTCPIPTask);
	
		// Start of the RTOS scheduler, this function should never return
		vTaskStartScheduler();
	}
	
	_dbgwrite("Unexpected end of program...\r\n");

	while(1);
	return -1;
}

#if defined (FLYPORT_WF)
static DWORD tick01,tick02;
#endif
/*****************************************************************************
 FUNCTION 	TCPIPTask
			Main function to handle the TCPIP stack
 
 RETURNS  	None
 
 PARAMS		None
*****************************************************************************/
void TCPIPTask()
{
	WFConnection = WF_CUSTOM;
	ConnectionProfileID = 0;
	static DWORD dwLastIP = 0;
	_WFStat = NOT_CONNECTED;
	dwLastIP = 0;
	//	Function pointers for the callback function of the TCP/IP and WiFi stack 

#if defined (FLYPORT_WF)
	FP[1] = cWFConnect;
	FP[2] = cWFDisconnect;
	FP[3] = cWFScan;
	FP[5] = cWFPsPollDisable;
	FP[6] = cWFPsPollEnable;
	FP[7] = cWFScanList;
#if defined (FLYPORT_G)
	FP[8] = cRSSIUpdate;
	FP[9] = cWFGetPSK;
#endif
	FP[10] = cWFStopConnecting;
	
#endif
#if defined (FLYPORT_ETH)
	FP[1] = cETHRestart;
#endif
#if defined (STACK_USE_SSL_CLIENT)
	FP[14] = cTCPSSLStatus;
	FP[15] = cTCPSSLStart;
#endif
	FP[16] = cTCPRxFlush;
	FP[17] = cTCPpRead;
	FP[18] = cTCPRemote;
	FP[19] = cTCPServerDetach;
	FP[20] = cTCPGenericOpen;
	FP[21] = cTCPRead;
	FP[22] = cTCPWrite;
	FP[23] = cTCPGenericClose;
	FP[24] = cTCPisConn;
	FP[25] = cTCPRxLen;


	#if defined(STACK_USE_SMTP_CLIENT)
	FP[26] = cSMTPStart;
	FP[27] = cSMTPSetServer;
	FP[28] = cSMTPSetMsg;
	FP[29] = cSMTPSend;
	FP[30] = cSMTPBusy;
	FP[31] = cSMTPStop;
	FP[32] = cSMTPReport;
	#endif
	
	FP[ARP_RESOLVE] = cARPResolveMAC;
	#if MAX_UDP_SOCKETS_FREERTOS>0	
	FP[35] = cUDPGenericOpen;
	FP[36] = cUDPWrite;
	FP[37] = cUDPGenericClose;
    FP[38] = cUDPMultiOn;
	#endif
	
	// Initialize stack-related hardware components that may be 
	// required by the UART configuration routines
	
	// Initialization of tick and of DHCPs SM only at the startup of the device
	if (hFlyTask == NULL)
	{
	    TickInit();
	    #if defined STACK_USE_DHCP_SERVER
	    DHCPServerSMInit();
	    #endif
	}  
	#if defined(STACK_USE_MPFS) || defined(STACK_USE_MPFS2)
	MPFSInit();
	#endif

	// Initialize Stack and application related NV variables into AppConfig.
	InitAppConfig();

	// Initialize core stack layers (MAC, ARP, TCP, UDP) and application modules (HTTP, SNMP, etc.)
    StackInit();

	if (hFlyTask == NULL)
	{
		NETConf[0] = AppConfig;
		NETConf[1] = AppConfig;
	}
	
	#if defined(WF_CS_TRIS)
	//	On startup no connection profile should be present inside WiFi module, so a new one is created
	UINT8 listIds = 0;
	WF_CPGetIds(&listIds);
	if (listIds == 0)
	{
		WF_CPCreate(&ConnectionProfileID);
	}
	//	Logical connection state initialization
	SetLogicalConnectionState(FALSE);
    #endif



	#if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
    ZeroconfLLInitialize();
	#endif

	
	#if defined(STACK_USE_ZEROCONF_MDNS_SD)
	mDNSInitialize(MY_DEFAULT_HOST_NAME);
	mDNSServiceRegister(
		(const char *) "DemoWebServer",		// base name of the service
		"_http._tcp.local",			    	// type of the service
		80,				                	// TCP or UDP port, at which this service is available
		((const BYTE *)"path=/index.htm"),	// TXT info
		1,								    // auto rename the service when if needed
		NULL,							    // no callback function
		NULL							    // no application context
		);

    mDNSMulticastFilterRegister();			
	#endif

	//	INITIALIZING UDP
	#if MAX_UDP_SOCKETS_FREERTOS>0
	_dbgwrite("Initializing UDP...\r\n");
	UDPInit();
	activeUdpSocket=0;
	while (activeUdpSocket < MAX_UDP_SOCKETS_FREERTOS) 
	{
		tmp_len[activeUdpSocket]=0;
		if (activeUdpSocket == 0) 
		{
			BUFFER_UDP_LEN[0] = BUFFER1_UDP_LEN;
			udpBuffer[activeUdpSocket] = udpBuffer1;
			udpSocket[0] = INVALID_UDP_SOCKET;
		}
		#if MAX_UDP_SOCKETS_FREERTOS>1
		if (activeUdpSocket == 1)
		{
			BUFFER_UDP_LEN[1] = BUFFER2_UDP_LEN;
			udpBuffer[activeUdpSocket] = udpBuffer2;
			udpSocket[1] = INVALID_UDP_SOCKET;
		}
		#endif
		#if MAX_UDP_SOCKETS_FREERTOS>2
		if (activeUdpSocket == 2)
		{
			BUFFER_UDP_LEN[2] = BUFFER3_UDP_LEN;
			udpBuffer[activeUdpSocket] = udpBuffer3;
			udpSocket[2] = INVALID_UDP_SOCKET;
		}
		#endif
		#if MAX_UDP_SOCKETS_FREERTOS>3
		if (activeUdpSocket == 3)
		{
			BUFFER_UDP_LEN[3] = BUFFER4_UDP_LEN;
			udpBuffer[activeUdpSocket] = udpBuffer4;
			udpSocket[3] = INVALID_UDP_SOCKET;
		}
		#endif
		p_udp_wifiram[activeUdpSocket] = udpBuffer[activeUdpSocket];
		p_udp_data[activeUdpSocket] = udpBuffer[activeUdpSocket];
		activeUdpSocket++;
	}
	#endif
	if (hFlyTask == NULL)
	{
		//	Creates the task dedicated to user code
		xTaskCreate(FlyportTask,(signed char*) "FLY" , (configMINIMAL_STACK_SIZE * 4), 
		NULL, tskIDLE_PRIORITY + 1, &hFlyTask);	
	}
	//	DEBUG code - Firmware version on UART 1
	#ifdef FW_VER_ON_U1
	char fwVerString[30];
	tWFDeviceInfo deviceInfo;
	WF_GetDeviceInfo(&deviceInfo); 
	sprintf(fwVerString,"ver.%02x%02x\n", deviceInfo.romVersion , deviceInfo.patchVersion);
	_dbgwrite(fwVerString);
	#endif
//-------------------------------------------------------------------------------------------
//|							--- COOPERATIVE MULTITASKING LOOP ---							|
//-------------------------------------------------------------------------------------------
    while(1)
    {
        #if defined (FLYPORT_WF)
		if (_WFStat != TURNED_OFF)
		{

        #else
        {
        #endif
				
				
	        // This task performs normal stack task including checking
	        // for incoming packet, type of packet and calling
	        // appropriate stack entity to process it.
			vTaskSuspendAll();
			StackTask();
			xTaskResumeAll();
			#if defined(STACK_USE_HTTP_SERVER) || defined(STACK_USE_HTTP2_SERVER)
			vTaskSuspendAll();
			HTTPServer();
			xTaskResumeAll();
			#endif
			// This tasks invokes each of the core stack application tasks
	        StackApplications();
	
	        #if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
			ZeroconfLLProcess();
	        #endif
	
	        #if defined(STACK_USE_ZEROCONF_MDNS_SD)
	        mDNSProcess();
			// Use this function to exercise service update function
			// HTTPUpdateRecord();
	        #endif
	
			#if defined(STACK_USE_SNMP_SERVER) && !defined(SNMP_TRAP_DISABLED)
			//User should use one of the following SNMP demo
			// This routine demonstrates V1 or V2 trap formats with one variable binding.
			SNMPTrapDemo();
			#if defined(SNMP_STACK_USE_V2_TRAP)
			//This routine provides V2 format notifications with multiple (3) variable bindings
			//User should modify this routine to send v2 trap format notifications with the required varbinds.
			//SNMPV2TrapDemo();
			#endif 
			if(gSendTrapFlag)
				SNMPSendTrap();
			#endif
			
			#if defined(STACK_USE_BERKELEY_API)
			BerkeleyTCPClientDemo();
			BerkeleyTCPServerDemo();
			BerkeleyUDPClientDemo();
			#endif


			// Check on the queue to verify if other task have requested some stack function
			xStatus = xQueueReceive(xQueue,&Cmd,0);
			CmdCheck();
			#if defined (FLYPORT_WF)
			//	Check to verify the connection. If it's lost or failed, the device tries to reconnect
			switch(_WFStat)
			{
				case CONNECTION_LOST:
				case CONNECTION_FAILED:
					tick01 = TickGetDiv64K();
					_WFStat = RECONNECTING;		
					break;
				case RECONNECTING:
					tick02 = TickGetDiv64K();
					if ((tick02 - tick01) >= 3)
					{
						_WFStat = CONNECTING;
						WF_Connect(WFConnection);
					}	
					break;
			}
			//	RSSI management
			if (myRSSI.stat == RSSI_TO_READ)
			{
				tWFScanResult rssiScan;
				WF_ScanGetResult(0, &rssiScan);
				myRSSI.value = rssiScan.rssi;
				myRSSI.stat = RSSI_VALID;
			}	
			#endif
	        // If the local IP address has changed (ex: due to DHCP lease change)
	        // write the new IP address to the LCD display, UART, and Announce 
	        // service
			if(dwLastIP != AppConfig.MyIPAddr.Val)
			{
				dwLastIP = AppConfig.MyIPAddr.Val;
				
				_dbgwrite("\r\nNew IP Address: ");

				DisplayIPValue(AppConfig.MyIPAddr);
	
				_dbgwrite("\r\n");
	
				#if defined(STACK_USE_ANNOUNCE)
					AnnounceIP();
				#endif
	
	            #if defined(STACK_USE_ZEROCONF_MDNS_SD)
					mDNSFillHostRecord();
				#endif
			}
		} //end check turnoff	
	}
}

