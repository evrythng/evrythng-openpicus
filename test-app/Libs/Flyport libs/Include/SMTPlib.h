/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        SMTPlib.h
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
#ifndef __SMTP_LIB_H
#define __SMTP_LIB_H

// TCPIP stack includes
#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/SMTP.h"


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
extern SMTP_POINTERS SMTPClient;


//	RTOS components - Semaphore and queues
extern xQueueHandle xQueue;
extern xSemaphoreHandle xSemFrontEnd;

#if defined(STACK_USE_SMTP_CLIENT)
 BOOL SMTPStart();
 int cSMTPStart();
#endif

#define SERVER_NAME		0
#define SERVER_USER		1
#define SERVER_PASS		2
#define SERVER_PORT		3

#define MSG_TO			0
#define MSG_CC			1
#define MSG_BCC			2
#define MSG_FROM		3
#define MSG_SUBJECT		4
#define	MSG_BODY		5

void SMTPSetServer(int , char *);
int cSMTPSetServer();

void SMTPSetMsg(int , char *);
int cSMTPSetMsg();

BOOL GenericSMTP(int);

BOOL SMTPSend();
int cSMTPSend();

BOOL SMTPBusy();
int cSMTPBusy();

BOOL SMTPStop();
int cSMTPStop();

WORD SMTPReport();
int cSMTPReport();

#endif
