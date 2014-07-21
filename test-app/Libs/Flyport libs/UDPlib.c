/** \file UDPlib.c
 *  \brief UDP wrapper for FreeRTOS
 *
 * \addtogroup net
 * @{
*/

/**
\defgroup UDPlib UDPlib stack
@{

UDP provides the commands to manage UDP connections. Flyport supports the creation of two different UDP socket, with their own RX buffer.

\section UDPlib UDP library
*/

/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        UDPlib.c
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

#include "UDPlib.h"

#if MAX_UDP_SOCKETS_FREERTOS>0

static int ToSend = 0; 
extern BOOL MACLinked;

/// @cond debug
//*****************************************************************************************
// Only internal use:
// 35 - UDPGenericOpen and callback function: opens a generic UDP socket
//*****************************************************************************************
BYTE UDPGenericOpen(char localport[], DWORD remhost, char remoteport[], BYTE connType )
{
	BYTE retsock;
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);	//	xSemFrontEnd TAKE
	xErr = 0;
	if (xFrontEndStat == 0)
	{	
		//	OpCode and requested return value from the stack
		xFrontEndStatRet = 2;	
		ToSend = 35;
		
		//	Passing function parameters
		//	The remote host type is always UDP_OPEN_RAM_HOST, so it's passed as pointer to a char
		xUDPLocalPort = atoi(localport);				//	Local port is set to 0 by calling functions, so it's managed by TCP/IP stack
		xUDPRemotePort = atoi(remoteport);				//	Remote port is passed directly by calling function				
		xUDPRemoteHost = (DWORD)(unsigned int)remhost;	//	Double casting to make compiler happy
		xByte2 = connType;								// Connection type, containg the format of the server
		
		//	Queue and status setting, callback is ready to be fired when semaphore is released
		xQueueSendToBack(xQueue,&ToSend,0);				//	Send COMMAND to the stack
		xFrontEndStat = 1;
		xSemaphoreGive(xSemFrontEnd);					//	xSemFrontEnd GIVE
	}
	else
	{
		xErr = 35; 	// Error code = ToSend, it can be changed from callback function to manage different cases
		xSemaphoreGive(xSemFrontEnd);
		taskYIELD();
		return 0;	//Socket creation error
	}
	
	while (xFrontEndStat != 2);							//	Waits for stack answer
	//	Stack performed the callback, reading the answer
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);	//	xSemFrontEnd TAKE
	retsock = callbackUdpSocket;						//	The returning UDP socket, opened by callback
	xFrontEndStat = 0;		
	xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE
	taskYIELD();
	return retsock;
}

//	UDPGenericOpen callback function
int cUDPGenericOpen()
{
	BYTE count = 0;
	UDP_SOCKET tmp_sock = INVALID_UDP_SOCKET;

	udpInt = 0;
	//tmp_sock = UDPOpen(xUDPLocalPort, xUDPRemoteHost, xUDPRemotePort);
	tmp_sock = UDPOpenEx(xUDPRemoteHost, xByte2, xUDPLocalPort, xUDPRemotePort);
	if (tmp_sock == INVALID_UDP_SOCKET)
	{
		callbackUdpSocket = 0;
		return 1; // Error;
	}
	else
	{
		numUdpSocket++;
		while (count < MAX_UDP_SOCKETS_FREERTOS)
		{
			if (udpSocket[count] == INVALID_UDP_SOCKET)
			{
				udpSocket[count] = tmp_sock;
				xUDPPort[count] = UDPSocketInfo[tmp_sock].localPort;
				count++;
				callbackUdpSocket = count;
				return 0; // Open succes
			}
			count++;
		}
		callbackUdpSocket = 0;
		return 1; //Error
	}
	
}
/// @endcond


/**
 * Returns the local port of the specified UDP socket. It can be useful when using a socket as client.
 * \param udplocalsocket the number of the UDP socket.
 * \return The number of current socket, or 0 if an error occured during the opening of the socket.
 */
WORD UDPLocalPort (BYTE udplocalsocket)
{			
	WORD dummyPort;
	dummyPort = xUDPPort[udplocalsocket - 1];
	return dummyPort;
}


/**
 * Create a UDP server on specified port
 * \param udpport Local Port for UDP server
 * \return The number of current socket, or 0 if an error occured during the opening of the socket.
 */
BYTE UDPServerOpen (char udpport[])
{			
	return UDPGenericOpen(udpport, 0 , udpport, UDP_OPEN_SERVER);
}

/**
 * Create a UDP client on specified port, try more time for arp request.
 * \param udpaddr IP address of server
 * \param udpport Remote Port of UDP server
 * \return The number of current socket or 0 if an error occured during the opening of the socket.
 */
BYTE UDPClientOpen (char udpaddr[], char udpport[])
{
    #if defined (FLYPORT_WF)
	if (_WFStat != TURNED_OFF)
	#elif defined (FLYPORT_ETH)
	if (MACIsLinked())
	#endif
	{
		return UDPGenericOpen(0, (DWORD)(unsigned int)udpaddr, udpport, UDP_OPEN_RAM_HOST);
	}
	return 0;
}

/**
 * Create a UDP broadcast on specified port
 * \param udpport Remote Port for UDP
 * \return The number of current socket or 0 if an error occured during the opening of the socket.
 */
BYTE UDPBroadcastOpen (char udpport[])
{			
	return UDPGenericOpen(0, NULL, udpport, UDP_OPEN_RAM_HOST);
}


/// @cond debug
//*****************************************************************************************
// Only internal use
// 37 - UDPGenericClose and callback function: close an open UDP socket
//*****************************************************************************************
BYTE UDPGenericClose(BYTE sock)
{
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
	xErr = 0;
	if (xFrontEndStat == 0)
	{	
		xFrontEndStatRet = 2;
		ToSend=37;
		callbackUdpSocket = sock;
		
		xQueueSendToBack(xQueue,&ToSend,0);					//	Send COMMAND to the stack
		xFrontEndStat = 1;
		xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE	
		while (xFrontEndStat != 2);							//	Waits for stack answer
		while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);	//	xSemFrontEnd TAKE
		xFrontEndStat = 0;
		xSemaphoreGive(xSemFrontEnd);	
		taskYIELD();
	}
	else
	{
		xErr = 37;
		xSemaphoreGive(xSemFrontEnd);
		taskYIELD();
	}
	return 0;
}

//	Callback function
int cUDPGenericClose()
{
	UDPClose(udpSocket[callbackUdpSocket-1]);
	udpSocket[callbackUdpSocket-1] = INVALID_UDP_SOCKET;
	numUdpSocket--;
	return 0;
}
/// @endcond


/**
 * Closes UDP Server socket
 * \param sock UDP Socket number
 */
BYTE UDPServerClose(BYTE sock) 
{
	return UDPGenericClose(sock);
}

/**
 * Closes UDP Client socket
 * \param sock UDP Socket number
 */
BYTE UDPClientClose(BYTE sock) 
{
	return UDPGenericClose(sock);
}

/**
 * Reads the length of the RX buffer
 * \param sock UDP socket number
 * \return The number of char that can be read from the UDP buffer.
 */
WORD UDPRxLen(BYTE sock)
{
	return udpRxLenGlobal[sock-1];
}

/**
 * Empty the RX buffer
 * \param sock UDP socket number
 * \return none
 */
void UDPRxFlush(BYTE sock)
{
	BYTE socktmp;
	socktmp = sock - 1;
	p_udp_wifiram[socktmp] = udpBuffer[socktmp];
	p_udp_data[socktmp] = p_udp_wifiram[socktmp];
	*p_udp_data[socktmp] = 0;
	udpRxLenGlobal[socktmp] = 0;
}

/**
 * Checks if a overflow was reached in UDP RX buffer, and clear the flag. 
 * \param sock UDP socket number
 * \return 0 = no overflow, 1 = overflow reached
 */
BOOL UDPRxOver(BYTE sock)
{
	if(UDPoverflowFlag[sock-1] == 1)
	{
		UDPoverflowFlag[sock-1] = 0;
		return 1;
	}
	else
		return 0;
}

/**
 * Reads lstr bytes from the RX buffer
 * \param sock UDP socket number
 * \param str2rd Buffer for data
 * \param lstr lenght of string
 * \return The number of read characters from the specified UDP socket.
 */
int UDPRead(BYTE sock, char str2rd[] , int lstr)
{
    #if defined (FLYPORT_WF)
	if (_WFStat != TURNED_OFF)
	#elif defined (FLYPORT_ETH)
	if (MACIsLinked())
	#endif
	{
		BYTE socktmp;
		int tmpread = 0;
		
		socktmp = sock-1;
		if (lstr > BUFFER_UDP_LEN[socktmp])  //overflow
			lstr = BUFFER_UDP_LEN[socktmp];
		
		//doppiaggio dei dati da testare con pulsante
		if (udpRxLenGlobal[socktmp] > BUFFER_UDP_LEN[socktmp])
		{
			udpRxLenGlobal[socktmp] = BUFFER_UDP_LEN[socktmp];
			p_udp_data[socktmp] = p_udp_wifiram[socktmp];
		}

		while(tmpread < lstr)
		{
			str2rd[tmpread] = *p_udp_data[socktmp];
			if ( p_udp_data[socktmp] < (udpBuffer[socktmp]+(BUFFER_UDP_LEN[socktmp]-1)) )
			{
				p_udp_data[socktmp]++;
			}
			else
			{
				p_udp_data[socktmp] = udpBuffer[socktmp];
			}
			tmpread++;
		}
		udpRxLenGlobal[socktmp] -= tmpread;
		return tmpread;
	}
	return 0;
}

/**
 * Reads lstr bytes from the RX buffer without clear it
 * \param sock UDP socket number
 * \param str2rd Buffer for data
 * \param lstr lenght of string
 * \param start point of start reading (in byte)
 * \return The number of read characters from the specified UDP socket.
 */
int UDPpRead(BYTE sock, char str2rd[], int lstr, int start)
{
	#if defined (FLYPORT_WF)
	if (_WFStat != TURNED_OFF)
	#elif defined (FLYPORT_ETH)
	if (MACIsLinked())
	#endif
	{
		BYTE socktmp;
		socktmp = sock-1;
		int tmpread = 0;
		BYTE* peek;
		peek = p_udp_data[socktmp]+start;
		
		
		if (lstr > (BUFFER_UDP_LEN[socktmp]-start))  //overflow
			lstr = BUFFER_UDP_LEN[socktmp]-start;
		
		//doppiaggio dei dati da testare con pulsante
		if (udpRxLenGlobal[socktmp] > BUFFER_UDP_LEN[socktmp])
		{
			udpRxLenGlobal[socktmp] = BUFFER_UDP_LEN[socktmp];
			peek = p_udp_wifiram[socktmp];
		}

		while(tmpread < lstr)
		{
			str2rd[tmpread] = *peek;
			if ( peek < (udpBuffer[socktmp]+(BUFFER_UDP_LEN[socktmp]-1)) )
			{
				peek++;
			}
			else
			{
				peek = udpBuffer[socktmp];
			}
			tmpread++;
		}
		//udpRxLenGlobal[socktmp] -= tmpread;
		return tmpread;
	}
	return 0;
}	

/**
 * Writes on the UDP socket
 * \param sockwr UDP socket number
 * \param str2wr String to write
 * \param lstr String lenght
 * \return The number of write characters to the specified UDP socket.
 */
WORD UDPWrite(BYTE sockwr, BYTE* str2wr , int lstr)
{
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
	xErr = 0;
	if (xFrontEndStat == 0)
	{	
		ToSend = 36;
		xFrontEndStatRet = 2;
		callbackUdpSocket = sockwr;
		udpByte = str2wr;
		udpInt = lstr;
		xQueueSendToBack(xQueue,&ToSend,0);					//	Send UDPWrite command to the stack
		xFrontEndStat = 1;
		xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE	
	}
	else
	{
		xErr = 36;
		xSemaphoreGive(xSemFrontEnd);
		taskYIELD();
		return FALSE;
	}
	
	while (xFrontEndStat != 2);								//	Waits for stack answer
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
	WORD resconn;
	resconn = udpWord;
	xFrontEndStat = 0;
	xSemaphoreGive(xSemFrontEnd);							//	xSemFrontEnd GIVE
	taskYIELD();
	return resconn;
}

/// @cond debug
int cUDPWrite()
{
	//reads udp data and adds in ring buffer
	if ( UDPIsPutReady(udpSocket[callbackUdpSocket-1]) )
	{
		udpWord = UDPPutArray(udpByte,udpInt);
		UDPFlush();
		return 0;
	} else {
		udpWord = 0;
		return 1; //error
	}
}
/// @endcond


/********************************************
 * Multicast section
 * *****************************************/

void UDPMultiOn(char *multiaddress)
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
			ToSend = 38;

			strncpy((char*)xIPAddress, multiaddress, strlen(multiaddress));

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
				return;
		}
	}

	while (xFrontEndStat != 2);					//	Waits for stack answer
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE

	xFrontEndStat = 0;
	xSemaphoreGive(xSemFrontEnd);                                   //	xSemFrontEnd GIVE
	taskYIELD();

}

typedef struct
{
    BYTE type;
    BYTE maxtime;
    int checksum;
    IP_ADDR maddr;
} IGMP_PACKET;


int cUDPMultiOn()
{
    IP_ADDR m_IP;
    IGMP_PACKET IGMPPacket;

    StringToIPAddress((BYTE*)xIPAddress, &m_IP);

#if defined (FLYPORT_WF)
    UINT8 multiMAC[6];
    
    WF_SetMultiCastFilter(5, multiMAC);
#endif
     
#if defined (FLYPORT_ETH)
    MAC_ADDR multiMAC;
    multiMAC.v[0] = 0x01;
    multiMAC.v[1] = 0x00;
    multiMAC.v[2] = 0x5E;
    multiMAC.v[3] = (UINT8)m_IP.v[1];
    multiMAC.v[4] = (UINT8)m_IP.v[2];
    multiMAC.v[5] = (UINT8)m_IP.v[3];
    SetRXHashTableEntry(multiMAC);
#endif
    // IGMP query

    if (!IPIsTxReady())
        return -1;
    else
    {
        // Definition of the IGMP node
        NODE_INFO IGMPNode;
        IGMPNode.IPAddr.v[0] = m_IP.v[0];
        IGMPNode.IPAddr.v[1] = m_IP.v[1];
        IGMPNode.IPAddr.v[2] = m_IP.v[2];
        IGMPNode.IPAddr.v[3] = m_IP.v[3];
        IGMPNode.MACAddr.v[0] = 0x01;
        IGMPNode.MACAddr.v[1] = 0x00;
        IGMPNode.MACAddr.v[2] = 0x5e;
        IGMPNode.MACAddr.v[3] = m_IP.v[1];
        IGMPNode.MACAddr.v[4] = m_IP.v[2];
        IGMPNode.MACAddr.v[5] = m_IP.v[3];

        IGMPPacket.type = 0x16;
        IGMPPacket.maxtime = 0x00;
        IGMPPacket.checksum = 0x00;
        IGMPPacket.maddr.v[0] = m_IP.v[0];
        IGMPPacket.maddr.v[1] = m_IP.v[1];
        IGMPPacket.maddr.v[2] = m_IP.v[2];
        IGMPPacket.maddr.v[3] = m_IP.v[3];

        IGMPPacket.checksum = CalcIPChecksum((BYTE*)&IGMPPacket, sizeof(IGMPPacket));

        MACSetWritePtr(BASE_TX_ADDR + sizeof(ETHER_HEADER));
        // IP header
        IPPutHeader(&IGMPNode, IP_PROT_IGMP, sizeof(IGMPPacket) );

        // IP IGMP data
        MACPutArray((BYTE*)&IGMPPacket, sizeof(IGMPPacket));
        MACFlush();
    }

     
    return 0;
}


BYTE UDPMultiOpen (char *udpmultiaddr, char udpmultiport[])
{
    #if defined (FLYPORT_WF)
	if (_WFStat != TURNED_OFF)
	#elif defined (FLYPORT_ETH)
	if (MACIsLinked())
	#endif
    {
        static NODE_INFO MulticastNode;
		DWORD argNode;
        // Enabling the multicast address
        UDPMultiOn(udpmultiaddr);
        // Creating the remote node and opening the socket
        StringToIPAddress((BYTE*) udpmultiaddr, &MulticastNode.IPAddr);

        MulticastNode.MACAddr.v[0] = 0x01;
        MulticastNode.MACAddr.v[1] = 0x00;
        MulticastNode.MACAddr.v[2] = 0x5E;
        MulticastNode.MACAddr.v[3] = MulticastNode.IPAddr.v[1];
        MulticastNode.MACAddr.v[4] = MulticastNode.IPAddr.v[2];
        MulticastNode.MACAddr.v[5] = MulticastNode.IPAddr.v[3];
		argNode = (DWORD) (unsigned int)&MulticastNode;
        return UDPGenericOpen(0, argNode, udpmultiport, UDP_OPEN_NODE_INFO);
    }
    return 0;
}
#endif
/** @} */
/** @} */
