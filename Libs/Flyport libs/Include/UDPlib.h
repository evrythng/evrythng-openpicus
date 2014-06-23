/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        UDPlib.h
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Andrea Seraghiti	 1.0	 1/20/2011         First release (core team)
 *  Andrea Seraghiti	 1.1	 2/12/2011         Fixed UPDRead, reuse of
 *                                                 a closed socket, changed
 *												   ARPResolve control in UDPclient
 *  Simone Marra		 1.2     3/09/2012         Added UDPRxOver, UDPpRead 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Software License Agreement
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  This is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by 
 *  the Free Software Foundation AND MODIFIED BY OpenPicus team.
 *  
 *  ***NOTE*** The exception to the GPL is included to allow you to distribute
 *  a combined work that includes OpenPicus code without being obliged to 
 *  provide the source code for proprietary components outside of the OpenPicus
 *  code. 
 *  OpenPicus software is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details. 
 * 
 * 
 * Warranty
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * WE ARE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 **************************************************************************/
#ifndef __UDPLIB_H
#define __UDPLIB_H

#include "ARPlib.h"

#define MAX_UDP_SOCKETS_FREERTOS (1u)

#define BUFFER_UDP_FIXED_LEN 50
#define BUFFER1_UDP_LEN 200
#define BUFFER2_UDP_LEN BUFFER_UDP_FIXED_LEN
#define BUFFER3_UDP_LEN BUFFER_UDP_FIXED_LEN
#define BUFFER4_UDP_LEN BUFFER_UDP_FIXED_LEN

//UDP variables:
extern WORD BUFFER_UDP_LEN[MAX_UDP_SOCKETS_FREERTOS];
extern BYTE* udpBuffer[MAX_UDP_SOCKETS_FREERTOS];
extern WORD udpRxLenGlobal[MAX_UDP_SOCKETS_FREERTOS];
extern BYTE numUdpSocket;
extern UDP_SOCKET udpSocket[MAX_UDP_SOCKETS_FREERTOS];
extern UDP_PORT xUDPPort[MAX_UDP_SOCKETS_FREERTOS];
extern BYTE callbackUdpSocket;
extern 	BYTE* p_udp_data[MAX_UDP_SOCKETS_FREERTOS];
extern 	BYTE* p_udp_wifiram[MAX_UDP_SOCKETS_FREERTOS];
extern DWORD xUDPRemoteHost;
extern WORD xUDPLocalPort;
extern WORD xUDPRemotePort;
extern WORD udpWord;
extern int udpInt;
extern BYTE* udpByte;


extern int udpFrontEndStat;
extern int udpFrontEndStatRet;
extern BOOL udpBool;
extern BOOL UDPoverflowFlag[MAX_UDP_SOCKETS_FREERTOS];
extern BYTE xByte2;
//	Frontend variables
extern int xFrontEndStat;
extern int xFrontEndStatRet;
extern int xErr;

//	RTOS components - Semaphore and queues
extern xQueueHandle xQueue;
extern xSemaphoreHandle xSemFrontEnd;

BYTE UDPClientOpen (char udpaddr[], char udpport[]);
BYTE UDPServerOpen (char udpport[]);
BYTE UDPBroadcastOpen (char udpport[]);

BYTE UDPClientClose(BYTE sock);
BYTE UDPServerClose(BYTE sock);

WORD UDPWrite(BYTE , BYTE* , int);
int cUDPWrite();

WORD UDPLocalPort(BYTE);

WORD UDPRxLen(BYTE);
void UDPRxFlush(BYTE sock);
BOOL UDPRxOver(BYTE);
int UDPRead(BYTE, char*, int);
int UDPpRead(BYTE, char*, int, int);

//internal use
BYTE UDPGenericOpen(char localport[], DWORD remhost, char remoteport[], BYTE connType);
int cUDPGenericOpen();

BYTE UDPGenericClose(BYTE);
int cUDPGenericClose();

void UDPMultiOn(char* multiaddres);
int cUDPMultiOn();

BYTE UDPMultiOpen(char *udpmultiaddr, char udpmultiport[]);

#endif // #ifndef __UDPLIB_H
