
/* **************************************************************************
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 *
 *            openSource wireless Platform for sensors and Internet of Things
 * **************************************************************************
 *  FileName:        INTlib.h
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Gabriele Allegria	1.0	16/08/2011	First Release
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

#ifndef __NET_LIB_H
#define __NET_LIB_H
#include "HWmap.h"
#include "GenericTypeDefs.h"
void NETCustomSave();
void NETCustomDelete();
void NETCustomLoad();
BOOL NETCustomExist();
void NETSetParam(int , char []);

#define MY_IP_ADDR		0
#define PRIMARY_DNS		1
#define SECONDARY_DNS   2
#define MY_GATEWAY		3
#define SUBNET_MASK		4
#define NETBIOS_NAME    5
#define DHCP_ENABLE		6

#define ENABLED		"On"
#define	DISABLED	"Off"


#if defined (FLYPORT_ETH)
void ETHRestart(int);
int cETHRestart();
#endif


#if defined (FLYPORT_WF)
// TCPIP stack includes
#include "WF_Config.h"
#include "TCPIP Stack/TCPIP.h"


	
#define SSID_NAME		7
#define NETWORK_TYPE	8
#define BSSID			9

#define INFRASTRUCTURE 	"infra"
#define ADHOC			"adhoc"
#if defined FLYPORT_G
#define SOFTAP			"softap"
#endif

#define WFCustomSave 		NETCustomSave
#define WFCustomDelete 		NETCustomDelete
#define WFCustomLoad		NETCustomLoad
#define WFCustomExist		NETCustomExist
#define WFSetParam			NETSetParam

#define WFCH_1		0x0001
#define WFCH_2		0x0002
#define WFCH_3		0x0004
#define WFCH_4		0x0008
#define WFCH_5		0x0010
#define WFCH_6		0x0020
#define WFCH_7		0x0040
#define WFCH_8		0x0080
#define WFCH_9		0x0100
#define WFCH_10		0x0200
#define WFCH_11		0x0400
#define WFCH_ALL	0x07FF


extern int WFNetworkFound;

//	Struct that will contain the characteristic of the scanned networks
typedef struct
{
	BYTE bssid[WF_BSSID_LENGTH];
	char ssid[WF_MAX_SSID_LENGTH+1];
	UINT8 channel;
	UINT8 signal;
	BYTE security;
	BYTE type;
	UINT8 beacon;
	UINT8 preamble;
	
}tWFNetwork;

void WFGeneric(int); 

void WFConnect(int);
int cWFConnect();

void WFScan();
int cWFScan();

#ifdef FLYPORT_G
void RSSIUpdate();
int cRSSIUpdate();
int RSSIValue();
BYTE RSSIStatus();
BOOL WFGetPSK(char *myPsk);
int cWFGetPSK();
#endif

void WFDisconnect();
int cWFDisconnect();

void WFSetChannel(unsigned int chSets);
void WFSetSecurity(BYTE , char* , BYTE , BYTE);

void WFStopConnecting();
int cWFStopConnecting();

tWFNetwork WFScanList(int);
int cWFScanList();

void WFHibernate();
void WFSleep();
void WFOn();


void WFPsPollEnable(BOOL ps_active);
int cWFPsPollDisable();
int cWFPsPollEnable();

int WFGetStat();
#endif

#endif
