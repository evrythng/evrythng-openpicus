/** \file TCPlib.c
 *  \brief TCP wrapper for FreeRTOS
 */

/**
\addtogroup net
@{
*/

/**
\defgroup TCPlib TCPlib stack
@{

The TCP library contains all the command to manage the TCP sockets. If you need to use a TCP client or server inside your application, this is the correct section.

\section TCP library
*/


/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        TCPlib.c
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Gabriele Allegria    1.0     1/20/2011		   First release  (core team)
 *  
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

#include "TCPlib.h"

static int ToSend = 0; 
/// @cond debug

#if defined (STACK_USE_SSL_CLIENT)
BYTE TCPSSLStatus(TCP_SOCKET sslclient)
{
    BOOL opok = FALSE;
    while (!opok)
    {
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
	if (xFrontEndStat == 0)
	{
	    ToSend = 14;
	    xFrontEndStatRet = 2;
	    xSocket = sslclient;
	    xQueueSendToBack(xQueue,&ToSend,0);					//	Send TCPRxLen request to the stack
	    xFrontEndStat = 1;
	    xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE
	    opok = TRUE;
	}
	else
	{
	    xSemaphoreGive(xSemFrontEnd);
	    taskYIELD();
	    //	If WiFi module if turned OFF, function doesn't do anything
	    if (xFrontEndStat == -1)
		return 1;
	}
    }

    while (xFrontEndStat != 2);								//	Waits for stack answer
    while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
    BYTE sslStatus;
    sslStatus = xByte2;
    xFrontEndStat = 0;
    xSemaphoreGive(xSemFrontEnd);
    return sslStatus;
}

int cTCPSSLStatus()
{
    if (TCPSSLIsHandshaking(xSocket))
		xByte2 = 1;
    else if (TCPIsSSL(xSocket))
		xByte2 = 2;
    else
		xByte2 = 0;
    return 0;
}

BYTE TCPSSLStart(TCP_SOCKET sslsock)
{
    BOOL opok = FALSE;
    while (!opok)
    {
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
	if (xFrontEndStat == 0)
	{
	    ToSend = 15;
	    xFrontEndStatRet = 2;
	    xSocket = sslsock;
	    xQueueSendToBack(xQueue,&ToSend,0);					//	Send TCPRxLen request to the stack
	    xFrontEndStat = 1;
	    xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE
	    opok = TRUE;
	}
	else
	{
	    xSemaphoreGive(xSemFrontEnd);
	    taskYIELD();
	    //	If WiFi module if turned OFF, function doesn't do anything
	    if (xFrontEndStat == -1)
		return 1;
	}
    }

    while (xFrontEndStat != 2);								//	Waits for stack answer
    while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
    BYTE sslStartStat;
    sslStartStat = xByte2;
    xFrontEndStat = 0;
    xSemaphoreGive(xSemFrontEnd);							//	xSemFrontEnd GIVE
    //taskYIELD();
    return sslStartStat;
}

int cTCPSSLStart()
{
    if(TCPStartSSLClient(xSocket, (BYTE*) "2"))
    {
		xByte2 = 0;
		return 0;
    }
    else
    {
		xByte2 = 2;
    }
    return 0;
}

#endif
//****************************************************************************
//	Only internal use:
//	TCPGenericOpen function: opens a generic TCP socket
//****************************************************************************
TCP_SOCKET TCPGenericOpen(char ipaddr[] , BYTE remhost , char tcpport[] , BYTE type)
{
	BOOL opok = FALSE;
	//	Function cycles until it is not executed
	while (!opok)
	{
		while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
		xErr = 0;
		//	Check on xFrontEndStat, if TCP stck is currently in use, is different from 0
		if (xFrontEndStat == 0)
		{	
			xFrontEndStatRet = 2;	
			ToSend=20;
		
			xTCPPort = atoi(tcpport);
			int termChar = strlen(ipaddr);
			if(termChar > 100) // xIPAddress array size is 100
				termChar = 100;
			strncpy((char*)xIPAddress, ipaddr, termChar);
			xIPAddress[termChar] = '\0';
			xByte2 = type;
			xByte3 = remhost;
			
			xQueueSendToBack(xQueue,&ToSend,0);					//	Send COMMAND request to the stack
			xFrontEndStat = 1;
			xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE, the stack can answer to the command
			opok = TRUE;
		}
		else
		{
			xSemaphoreGive(xSemFrontEnd);
			taskYIELD();
			//	If WiFi module if turned OFF, function doesn't do anything
			if (xFrontEndStat == -1)
				return INVALID_SOCKET;
		}
	}
		
	while (xFrontEndStat != 2);								//	Waits for stack answer
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
	TCP_SOCKET retsock;
	retsock = xSocket;
	xFrontEndStat = 0;
	xSemaphoreGive(xSemFrontEnd);							//	xSemFrontEnd GIVE, the stack can answer to the command
	taskYIELD();
	return retsock;
}

//****************************************************************************
//	Only internal use:
//	TCPGenericOpen callback function
//****************************************************************************
int cTCPGenericOpen()
{ 
	xSocket = INVALID_SOCKET;
	xSocket = TCPOpen((DWORD)&xIPAddress[0], xByte3 , xTCPPort, xByte2);
	return 0;
}


//****************************************************************************
//	Only internal use:
//	TCPGenericClose function: closes an open socket
//****************************************************************************
void TCPGenericClose(TCP_SOCKET sockclose)
{
	BOOL opok = FALSE;
	while (!opok)
	{
		while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
		xErr = 0;
		if (xFrontEndStat == 0)
		{	
			xFrontEndStatRet = 2;
			ToSend=23;
			xSocket = sockclose;
			xQueueSendToBack(xQueue,&ToSend,0);					//	Send COMMAND to the stack
			xFrontEndStat = 1;
			xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE, the stack can answer to the command	
		
			while (xFrontEndStat != 2);							//	Waits for stack answer
			while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);	//	xSemFrontEnd TAKE
			xFrontEndStat = 0;
			xSemaphoreGive(xSemFrontEnd);	
			opok = TRUE;
			taskYIELD();
		}
		else
		{
			xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE
			taskYIELD();
			//	If WiFi module if turned OFF, function doesn't do anything
			if (xFrontEndStat == -1)
				return;
		}
	}

}

//****************************************************************************
//	Only internal use:
//	TCPGenericClose callback function
//****************************************************************************
int cTCPGenericClose()
{
	TCPClose(xSocket);
	return 0;
}
/// @endcond



/**
 * Detaches the remote client from the server
 * \param sockdet - Socket of the server (the handle returned by the command TCPServerOpen).
 * \return None
 */
void TCPServerDetach(TCP_SOCKET sockdet)
{
	BOOL opok = FALSE;
	while (!opok)
	{
		while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
		xErr = 0;
		if (xFrontEndStat == 0)
		{	
			xFrontEndStatRet = 2;
			ToSend=19;
			xSocket = sockdet;
			xQueueSendToBack(xQueue,&ToSend,0);					//	Send COMMAND to the stack
			xFrontEndStat = 1;
			xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE	
			while (xFrontEndStat != 2);							//	Waits for stack answer
			while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);	//	xSemFrontEnd TAKE
			xFrontEndStat = 0;
			xSemaphoreGive(xSemFrontEnd);	
			opok = TRUE;
			taskYIELD();		
		}
		else
		{
			xSemaphoreGive(xSemFrontEnd);
			taskYIELD();
			//	If WiFi module if turned OFF, function doesn't do anything
			if (xFrontEndStat == -1)
				return;
		}
	}

}

/// @cond debug
//****************************************************************************
//	Only internal use:
//	TCPServerDetach callback function
//****************************************************************************
int cTCPServerDetach()
{
	TCPDisconnect(xSocket);
	return 0;
}
/// @endcond


/**
 * Creates a TCP client on specified IP address and port
 * \param tcpaddr - IP address of the remote server. Example: "192.168.1.100" (the char array must be NULL terminated).
 * \param tcpport - Port of the remote server to connect. Example: "1234" (the char array must be NULL terminated).
 * \return - INVALID_SOCKET: the operation was failed. Maybe there are not available sockets.
 * \return - A TCP_SOCKET handle to the created socket. It must be used to access the socket in the program (read/write operations and close socket).
 */
TCP_SOCKET TCPClientOpen (char tcpaddr[] , char tcpport[])
{
	return TCPGenericOpen(tcpaddr , 1 , tcpport , 0);
}


/**
 * Creates a TCP server on specified port 
 * \param tcpport - Number of the port for the server. Example: "1234" (the array must be NULL terminated).
 * \return - INVALID_SOCKET: the operation was failed. Maybe there are not available sockets.
 * \return - A TCP_SOCKET handle to the created socket. It must be used to access the socket in the program (read/write operations and close socket).
 */
TCP_SOCKET TCPServerOpen (char tcpport[])
{			
	return TCPGenericOpen("127.0.0.1" , 0 , tcpport , 1);
}


/**
 * Reads the specified number of characters from a TCP socket and puts them into the specified char array. <B>NOTE:</B> This function flushes the buffer after reading!
 * \param socktoread - The handle of the socket to read (the handle returned by the command TCPClientOpen or TCPServerOpen).
 * \param readch - The char array to fill with the read characters.
 * \param rlen - The number of characters to read. 
 * \warning The length of the array must be AT LEAST = rlen+1, because at the end of the operation the array it's automatically NULL terminated (is added the '\0' character).
 * \return None.
 */
void TCPRead(TCP_SOCKET socktoread , char readch[] , int rlen)
{
	BOOL opok = FALSE;
	while (!opok)
	{
		while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);			//	xSemFrontEnd TAKE
		xErr = 0;
		if (xFrontEndStat == 0)
		{	
			ToSend=21;
			xFrontEndStatRet = 2;
			xSocket = socktoread;
			xInt = rlen;
			xByte = (BYTE*)readch;
			xQueueSendToBack(xQueue,&ToSend,0);						//	Send READFTP command to the stack
			xFrontEndStat = 1;
			xSemaphoreGive(xSemFrontEnd);							//	xSemFrontEnd GIVE
			while (xFrontEndStat != 2);								//	Waits for stack answer
			while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE	
			xFrontEndStat = 0;
			xSemaphoreGive(xSemFrontEnd);	
			opok = TRUE;
			taskYIELD();
		}
		else
		{
			xSemaphoreGive(xSemFrontEnd);
			taskYIELD();
			//	If WiFi module if turned OFF, function doesn't do anything
			if (xFrontEndStat == -1)
				return;
		}
	}

}

/**
* Reads the specified number of characters from a TCP socket and puts them into the specified char array. <B>NOTE:</B> This function does flush the buffer!
* \param socktoread - The handle of the socket to read (the handle returned by the command TCPClientOpen or TCPServerOpen).
* \param readch - The char array to fill with the read characters.
* \param rlen - The number of characters to read. 
* \param start - The starting point to read.
* \warning The length of the array must be AT LEAST = rlen+1, because at the end of the operation the array it's automatically NULL terminated (is added the '\0' character).
* \return None.
*/
void TCPpRead(TCP_SOCKET socktoread , char readch[] , int rlen, int start)
{
	BOOL opok = FALSE;
	while (!opok)
	{
		while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);			//	xSemFrontEnd TAKE
		xErr = 0;
		if (xFrontEndStat == 0)
		{	
			ToSend=17;
			xFrontEndStatRet = 2;
			xSocket = socktoread;
			xInt = rlen;
			xInt2 = start;
			xByte = (BYTE*)readch;
			xQueueSendToBack(xQueue,&ToSend,0);						//	Send cTCPRead command to the stack
			xFrontEndStat = 1;
			xSemaphoreGive(xSemFrontEnd);							//	xSemFrontEnd GIVE
			while (xFrontEndStat != 2);								//	Waits for stack answer
			while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE	
			xFrontEndStat = 0;
			xSemaphoreGive(xSemFrontEnd);	
			opok = TRUE;
			taskYIELD();
		}
		else
		{
			xSemaphoreGive(xSemFrontEnd);
			taskYIELD();
			//	If WiFi module if turned OFF, function doesn't do anything
			if (xFrontEndStat == -1)
				return;
		}
	}
 }
 
 /// @cond debug
 //***************************************************************************
 //	 Only internal ude:
 //  cTCPpRead callback function
 //***************************************************************************
int cTCPpRead()
{
	WORD resbool;
	BYTE* tempByte;
	tempByte = (BYTE*)xByte;
	if(xInt2 > xInt)
		xInt2 = xInt;
	resbool = TCPPeekArray(xSocket, xByte, xInt, xInt2);
	if(resbool > 0)
	{
		*(tempByte+resbool) = '\0';
		return (int) resbool;
	}
	else
	{
		*(tempByte) = '\0';			
		return -1; //error value...
	}
}
/// @endcond


/// @cond debug
//****************************************************************************
//	Only internal use:
//	cTCPRead callback function
//****************************************************************************
int cTCPRead()
{
	WORD resbool;
	resbool = TCPGetArray(xSocket , xByte , xInt); 
	*(xByte+xInt)='\0';
	return (int) resbool;
}
/// @endcond


/**
 * Writes an array of characters on the specified socket.
 * \param socktowrite - The socket to which data is to be written (it's the handle returned by the command TCPClientOpen or TCPServerOpen).
 * \param writech - Pointer to the array of characters to be written.
 * \param wlen - The number of characters to write.
 * \return The number of bytes written to the socket. If less than len, the buffer became full or the socket is not conected.
 */
WORD TCPWrite(TCP_SOCKET socktowrite , char* writech , int wlen)
{
	BOOL opok = FALSE;
	while (!opok)
	{
		while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
		xErr = 0;
		if (xFrontEndStat == 0)
		{	
			ToSend = 22;
			xFrontEndStatRet = 2;
			xSocket = socktowrite;
			xByte = (BYTE*) writech;
			xInt = wlen;
			xQueueSendToBack(xQueue,&ToSend,0);					//	Send TCPWrite command to the stack
			xFrontEndStat = 1;
			xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE	
			opok = TRUE;
		}
		else
		{
			xSemaphoreGive(xSemFrontEnd);
			taskYIELD();
			//	If WiFi module if turned OFF, function doesn't do anything
			if (xFrontEndStat == -1)
				return 0;
		}
	}
	
	while (xFrontEndStat != 2);								//	Waits for stack answer
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
	WORD reswrite;
	reswrite = xWord;
	xFrontEndStat = 0;
	xSemaphoreGive(xSemFrontEnd);							//	xSemFrontEnd GIVE
	taskYIELD();
	return reswrite;

}

/// @cond debug
//****************************************************************************
//	Only internal use:
//	cTCPWrite callback function
//****************************************************************************
int cTCPWrite()
{
	xWord = TCPPutArray(xSocket , xByte , xInt);
	TCPFlush(xSocket);
	return xWord;
}
/// @endcond


/**
 * Writes an array of characters on the specified socket.
 * \param socktowrite - The socket to which data is to be written (it's the handle returned by the command TCPClientOpen or TCPServerOpen).
 * \param writech - Pointer to the array of characters to be written.
 * \param wlen - The number of characters to write.
 * \return The number of bytes written to the socket. If less than len, the buffer became full or the socket is not conected.
 */
NODE_INFO TCPRemote(TCP_SOCKET remotesock)
{
	BOOL opok = FALSE;
	while (!opok)
	{
		while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
		xErr = 0;
		if (xFrontEndStat == 0)
		{	
			ToSend = 18;
			xFrontEndStatRet = 2;
			xSocket = remotesock;
			xQueueSendToBack(xQueue,&ToSend,0);					//	Send TCPRemote command to the stack
			xFrontEndStat = 1;
			xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE	
			opok = TRUE;
		}
		else
		{
			xSemaphoreGive(xSemFrontEnd);
			taskYIELD();
			//	If WiFi module if turned OFF, function doesn't do anything
			if (xFrontEndStat == -1)
				return xNode;
		}
	}
	
	while (xFrontEndStat != 2);								//	Waits for stack answer
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
	NODE_INFO resremote;
	resremote = xNode;
	xFrontEndStat = 0;
	xSemaphoreGive(xSemFrontEnd);							//	xSemFrontEnd GIVE
	taskYIELD();
	return resremote;

}

/// @cond debug
//****************************************************************************
//	TCPRemote callback function
//****************************************************************************
int cTCPRemote()
{
	SOCKET_INFO* remote_sock;
	remote_sock = TCPGetRemoteInfo(xSocket);
	xNode = remote_sock -> remote;
	return 0;
}
/// @endcond




/**
 * Closes the client socket specified by the handle.
 * \param Sockclose - The handle of the socket to close (the handle returned by the command TCPClientOpen).
 * \return None.
 */
void TCPClientClose (TCP_SOCKET Sockclose)
{
	TCPGenericClose(Sockclose);
}


/**
 * Closes the server socket specified and destroys the handle. Any remote client connected with the server will be disconnected.
 * \param Sockclose - The handle of the socket to close (the handle returned by the command TCPServerOpen).
 * \return None.
 */
void TCPServerClose(TCP_SOCKET Sockclose)
{
	TCPGenericClose(Sockclose);
}



/**
 * Verifies the connection of a remote TCP device with the socket. It can be useful to catch an incoming new connection to a TCP server.
 * \param sockconn - The handle of the socket to control (the handle returned by the command TCPServerOpen).
 * \return TRUE - The remote connection is established.
 * \return FALSE - The remote connection is not established.
 */
BOOL TCPisConn(TCP_SOCKET sockconn)
{
	BOOL opok = FALSE;
	while (!opok)
	{
		while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
		if (xFrontEndStat == 0)
		{	
			ToSend = 24;
			xFrontEndStatRet = 2;
			xSocket = sockconn;
			xQueueSendToBack(xQueue,&ToSend,0);					//	Send TCPIsConnected command to the stack
			xFrontEndStat = 1;
			xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE	
			opok = TRUE;
		}
		else
		{
			xSemaphoreGive(xSemFrontEnd);
			taskYIELD();
			//	If WiFi module if turned OFF, function doesn't do anything
			if (xFrontEndStat == -1)
				return FALSE;
		}
	}
	
	while (xFrontEndStat != 2);								//	Waits for stack answer
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
	BOOL resconn;
	resconn = xBool;
	xFrontEndStat = 0;
	xSemaphoreGive(xSemFrontEnd);							//	xSemFrontEnd GIVE
	taskYIELD();
	return resconn;
}

/// @cond debug
//****************************************************************************
//	TCPIsConn callback function
//****************************************************************************
int cTCPisConn()
{
	xBool = TCPIsConnected(xSocket);
	return xBool;
}
/// @endcond


/**
 * Empty specified TCP socket RX Buffer.
 * \param sockflush - The handle of the socket to empty (the handle returned by the command TCPClientOpen or TCPServerOpen).
 * \return none.
 */
void TCPRxFlush(TCP_SOCKET sockflush)
{
	BOOL opok = FALSE;
	while (!opok)
	{
		while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
		xErr = 0;
		if (xFrontEndStat == 0)
		{	
			xFrontEndStatRet = 2;
			ToSend=16;
			xSocket = sockflush;
			xQueueSendToBack(xQueue,&ToSend,0);					//	Send COMMAND to the stack
			xFrontEndStat = 1;
			xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE, the stack can answer to the command	
		
			while (xFrontEndStat != 2);							//	Waits for stack answer
			while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);	//	xSemFrontEnd TAKE
			xFrontEndStat = 0;
			xSemaphoreGive(xSemFrontEnd);	
			opok = TRUE;
			taskYIELD();
		}
		else
		{
			xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE
			taskYIELD();
			//	If WiFi module if turned OFF, function doesn't do anything
			if (xFrontEndStat == -1)
				return;
		}
	}
}	

/// @cond debug
//****************************************************************************
//	TCPRxFlush callback function
//****************************************************************************
int cTCPRxFlush()
{
	TCPDiscard(xSocket);
	return 0;
}	
/// @endcond

/**
 * Verifies how many bytes can be read from the specified TCP socket.
 * \param socklen - The handle of the socket to control (the handle returned by the command TCPClientOpen or TCPServerOpen).
 * \return The number of bytes available to be read.
 */
WORD TCPRxLen(TCP_SOCKET socklen)
{
	BOOL opok = FALSE;
	while (!opok)
	{
		while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
		if (xFrontEndStat == 0)
		{	
			ToSend=25;
			xFrontEndStatRet = 2;
			xSocket = socklen;
			xQueueSendToBack(xQueue,&ToSend,0);					//	Send TCPRxLen request to the stack
			xFrontEndStat = 1;
			xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE	
			opok = TRUE;
		}
		else
		{
			xSemaphoreGive(xSemFrontEnd);
			taskYIELD();
			//	If WiFi module if turned OFF, function doesn't do anything
			if (xFrontEndStat == -1)
				return 0;
		}
	}
	
	while (xFrontEndStat != 2);								//	Waits for stack answer
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
	WORD reslen;
	reslen = xWord;
	xFrontEndStat = 0;
	xSemaphoreGive(xSemFrontEnd);							//	xSemFrontEnd GIVE
	//taskYIELD();
	return reslen;
}

/// @cond debug
//****************************************************************************
//	TCPRxLen callback function
//****************************************************************************
int cTCPRxLen()
{
	xWord = TCPIsGetReady(xSocket);
	return 0;
}
/// @endcond
