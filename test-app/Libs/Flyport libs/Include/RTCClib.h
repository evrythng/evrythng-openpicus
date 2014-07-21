/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        rtcc.h
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Stefano Saccucci     1.0     06/13/2011		   First release  
 *  Gabriele Allegria	 
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
#ifndef __RTCCLIB_H
#define __RTCCLIB_H
#include "HWlib.h"
#ifdef USE_RTCC_LIB
#include <time.h>

#define EVERY_HALF_SEC	0b0000
#define EVERY_SEC		0b0001
#define EVERY_TEN_SEC	0b0010
#define EVERY_MIN		0b0011
#define EVERY_TEN_MIN	0b0100
#define EVERY_HOUR		0b0101
#define EVERY_DAY		0b0110
#define EVERY_WEEK		0b0111
#define EVERY_MONTH		0b1000
#define EVERY_YEAR		0b1001

#define REPEAT_NO		0
#define REPEAT_INFINITE	256

#define SUNDAY		0
#define MONDAY		1
#define TUESDAY		2
#define WEDNESDAY	3
#define THURSADY	4
#define FRIDAY		5
#define SATURDAY	6

#define JANUARY		0
#define FEBRUARY	1
#define MARCH		2
#define APRIL		3
#define MAY			4
#define JUNE		5
#define JULY		6
#define AUGUST		7
#define SEPTEMBER	8
#define OCTOBER		9
#define NOVEMBER	10
#define DECEMBER	11

#define	NO_ALRM_EVENT	NULL

void RTCCSet(struct tm* rtcc);

void RTCCGet(struct tm* rtcc);

void RTCCAlarmConf(struct tm* rtcc, int repeats, BYTE mask, void (*fptr)());

void RTCCAlarmSet(BYTE run);

BOOL RTCCAlarmStat();
#endif
#endif
