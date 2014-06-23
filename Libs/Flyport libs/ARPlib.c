/** \file ARPlib.c
 *  \brief ARP wrapper for FreeRTOS
 *
 * \addtogroup net
 * @{
*/

/**
\defgroup ARPlib ARPlib stack
@{

ARP provides the commands to manage ARP-IP association. With ARPResolveMAC is possible to force an arp request,
but usually this operation in managed by TCP-IP Stack.

\section ARPlib ARP library
*/

/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        ARPlib.c
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Andrea Seraghiti	 1.0	 4/04/2011         First release
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

#include "ARPlib.h"

static int ToSend = 0; 

/**
 * Force an arp request for specific IP address
 * \param ipaddr IP address
 */
BYTE ARPResolveMAC(char ipaddr[])
{
	BYTE retsock;
	BOOL opok = FALSE;
	while (!opok)
	{
		while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
		xErr = 0;
		if (xFrontEndStat == 0)
		{	
			xFrontEndStatRet = 2;	
			ToSend=ARP_RESOLVE;
			memcpy((char*)xIPAddress,ipaddr,17);
			xQueueSendToBack(xQueue,&ToSend,0);					//	Send COMMAND to the stack
			xFrontEndStat = 1;
			xSemaphoreGive(xSemFrontEnd);						//	xSemFrontEnd GIVE	
			opok = TRUE;
		}
		else
		{
			xErr = ARP_RESOLVE; //ToSend;
			xSemaphoreGive(xSemFrontEnd);
			taskYIELD();
			//	If WiFi module if turned OFF, function doesn't do anything
			if (xFrontEndStat == -1)
				return 0;
		}
	}
	while (xFrontEndStat != 2);								//	Waits for stack answer
	while (xSemaphoreTake(xSemFrontEnd,0) != pdTRUE);		//	xSemFrontEnd TAKE
	retsock = 0;
	xFrontEndStat = 0;
	xSemaphoreGive(xSemFrontEnd);							//	xSemFrontEnd GIVE
	taskYIELD();
	return retsock;
}

/// @cond debug
//	ARPResolveMAC callback function
int cARPResolveMAC()
{	
	IP_ADDR dummyIP;
	dummyIP.Val = ( ((DWORD) xIPAddress[0]) +
					((DWORD) xIPAddress[1] << 8) +
					((DWORD) xIPAddress[2] << 16) +
					((DWORD) xIPAddress[3] << 24) );
	
	ARPResolve(&dummyIP);
	return 0;
}
/// @endcond

/** @} */
/** @} */
