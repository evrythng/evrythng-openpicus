/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        FTPlib.h
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
#ifndef __FTPLIB_H
#define __FTPLIB_H
#include "TCPlib.h"

//	Functions parameters defines
#define FTP_NO_PASS				NULL
#define STOR	"STOR "
#define APPE	"APPE "
#define RETR	"RETR "

//	Operation reports defines
#define FTP_FILE_EXIST				0
#define FTP_CONNECTED				0
#define FTP_ERR_NOT_CREATED			-1
#define FTP_ERR_SERV_NOT_FOUND		-2
#define FTP_ERR_WRONG_ANSWER		-3
#define FTP_ERR_WRONG_LOGIN			-4
#define FTP_ERR_SERV_TIMEOUT		-5
#define	FTP_STREAM_INVALID_OP		-6
#define FTP_DATA_NO_CONNECTED		-7
#define FTP_TX_BUFFER_TOO_SMALL 	-8
#define FTP_UNKNOWN_ERROR			-9
#define FTP_SOCK_NOT_CONNECTED		-10
#define FTP_ERR_SERV_DISCONNECTED	-11
#define FTP_FILE_ERROR				-12
#define FTP_ERR_TX					-13
#define FTP_ERR_TX_NOT_OK			-14
#define FTP_FILE_NOT_FOUND			-15

//	Stream status defines
#define FTP_STREAM_NOT_CONN			0
#define FTP_STREAM_READING			1
#define	FTP_STREAM_WRITING			2
#define	FTP_STREAM_EOF				3

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


//	RTOS components - Semaphore and queues
extern xQueueHandle xQueue;
extern xSemaphoreHandle xSemFrontEnd;

/*****************************************************************************
	FTP function declarations	
*****************************************************************************/
//	Low level functions
BOOL FTPisConn(TCP_SOCKET);
WORD FTPRxLen(TCP_SOCKET);
void FTPRead(TCP_SOCKET,char*,int);
WORD FTPWrite(TCP_SOCKET , BYTE* , int);
TCP_SOCKET FTPClientOpen (char* , char*);
void FTPClose(TCP_SOCKET);

//	Low level macro functions
TCP_SOCKET FTPClientPasv(TCP_SOCKET sockpasv, char ServerName[]);
int FTPSendCmd(TCP_SOCKET cmdSock, char *cmdStr, char* res, int resLen);
int FTPConnect(TCP_SOCKET *FTPConn, char *ftp_addr, char ftp_port[], char ftp_usr[], char ftp_pwd[]);

//	File management functions
int FTPFileCheck(TCP_SOCKET cmdSock, char fileToCheck[]);
long FTPFileSize(TCP_SOCKET cmdSock, char fileName[]);
int FTPAppend(TCP_SOCKET cmdSock, char fileName[], char appStr[]);
int FTPStore(TCP_SOCKET cmdSock, char fileName[], char stoStr[]);

//	Stream functions
int FTPStreamOpen(TCP_SOCKET cmdSock, char fileName[], char mode[]);
long FTPStreamWrite(char strWrite[], long toWrite);
long FTPStreamRead(char dest[], int len, BYTE timeout);
void FTPStreamClose();
BYTE FTPStreamStat();

//	Utils functions
int FTPAnswer(TCP_SOCKET FTPtoread, char* res_code);
int FTPRxFlush(TCP_SOCKET FTPtoflush, int timeout);
int FTPRxRead(TCP_SOCKET FTPtoflush, int timeout, char *buffer, int totLen);

#endif
