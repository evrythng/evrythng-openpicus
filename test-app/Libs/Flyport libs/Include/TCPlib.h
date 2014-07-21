/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        TCPlib.h
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Gabriele Allegria    1.0     1/20/2011		   First release  (core team)
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

#ifndef __TCP_LIB_H
#define __TCP_LIB_H

// TCPIP stack includes
//#include "WF_Config.h"
#include "TCPIP Stack/TCPIP.h"


//	Frontend variables
extern BYTE xIPAddress[];
extern WORD xTCPPort;
extern TCP_SOCKET xSocket;
extern int xFrontEndStat;
extern int xFrontEndStatRet;
extern int xErr;
extern BOOL xBool;
extern WORD xWord;
extern char *xChar;
extern BYTE *xByte;
extern BYTE xByte2;
extern BYTE xByte3;
extern int xInt;
extern int xInt2;
extern NODE_INFO xNode;


//	RTOS components - Semaphore and queues
extern xQueueHandle xQueue;
extern xSemaphoreHandle xSemFrontEnd;


/*****************************************************************************
	TCP function declarations	
*****************************************************************************/
int cTCPGenericOpen();
TCP_SOCKET TCPGenericOpen(char* , BYTE , char* , BYTE);

TCP_SOCKET TCPClientOpen (char* , char*);
TCP_SOCKET TCPServerOpen (char*);

void TCPClientClose(TCP_SOCKET);
void TCPServerClose(TCP_SOCKET);

int cTCPisConn();
BOOL TCPisConn(TCP_SOCKET);

int cTCPWrite();
WORD TCPWrite(TCP_SOCKET , char* , int);

int cTCPGenericClose();
void TCPGenericClose(TCP_SOCKET);

int cTCPRxLen();
WORD TCPRxLen(TCP_SOCKET);

int cTCPRxFlush();
void TCPRxFlush(TCP_SOCKET);

int cTCPRead();
void TCPRead(TCP_SOCKET, char*, int);

int cTCPpRead();
void TCPpRead(TCP_SOCKET, char*, int, int);

void TCPServerDetach(TCP_SOCKET);
int cTCPServerDetach();

NODE_INFO TCPRemote(TCP_SOCKET);
int cTCPRemote();
#if defined (STACK_USE_SSL_CLIENT)
BYTE TCPSSLStart(TCP_SOCKET sslsock);
int cTCPSSLStart();

BYTE TCPSSLStatus(TCP_SOCKET sslclient);
int cTCPSSLStatus();
#endif
#endif
