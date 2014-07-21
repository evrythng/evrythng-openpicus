/** \file HWlib.c
 *  \brief Hardware library to manage the analog and digital IOs and UART
 */

/**
\addtogroup Hardware
@{
*/
/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        RTCClib.c
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.                  		Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Stefano Saccucci     1.0     06/13/2011		Functions for register settings  
 *  Gabriele Allegria							New structure and functions 
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
/// @cond debug
#include "RTCClib.h"
#ifdef USE_RTCC_LIB

static WORD buffer = 0;
BOOL alarmflag = FALSE;
int arpt;
static void (*alarmFP)();


static WORD _BtoW(BYTE data1, BYTE data2);
static BYTE _BtoD(BYTE byte);
static BYTE _DtoB(BYTE dec);
/// @endcond


/**
\defgroup RTCC
@{
The RTCC library contains the command to manage the RTCC. It's possible to read/write the values
of date and time and set an alarm, including a callback function to execute when the alarm event 
is triggered.

*/


 /// @cond debug
/*---------------------------------------------------------------------------------------
| Function: 	void __attribute((interrupt,auto_psv)) _RTCCInterrupt()		            |
| Description: 	ISR for the RTCC alarm. If a callback function was set, it is executed.	|
| Returns:		-														   				|
| Parameters:	-													                	|
---------------------------------------------------------------------------------------*/
void __attribute((interrupt,auto_psv)) _RTCCInterrupt()
{
	while (RCFGCALbits.RTCSYNC == 1);
	arpt = ALCFGRPTbits.ARPT;
	if (arpt)
		RTCCAlarmSet(1);
	IFS3bits.RTCIF = 0;
	
	if (alarmFP != NULL)
		alarmFP();
	alarmflag = TRUE;  
}
/// @endcond

/**
 * Returns the status of the alarm event. The function can be polled continuously to get the alarm trigger.
 * \param None
 * \return the status of the alarm event:
  <UL>
	<LI><B>TRUE:</B> the alarm was triggered. The function is automatically set to FALSe until next alarm event.</LI> 
	<LI><B>FALSE:</B> no alarm event.</LI> 
 </UL>
 */
BOOL RTCCAlarmStat()
{
	if (alarmflag)
	{
		alarmflag = FALSE;
		return TRUE;
	}
	else
		return FALSE;
}


/**
 * Sets the date/time for the RTCC and enables the RTCC module.
 * \param rtcc - a pointer to a struct tm variable, containing the date and the time to set
 * \return None
 */
void RTCCSet(struct tm* rtcc)
{
	__builtin_write_OSCCONL(OSCCON | 0x02); 
	asm volatile("disi #5");
	__builtin_write_RTCWEN();
	
	ALCFGRPTbits.ALRMEN = 0;	
	RCFGCALbits.RTCEN = 0;
	
	RCFGCALbits.RTCPTR = 3;
	
	RTCVAL = _DtoB(rtcc->tm_year - 100);
	RTCVAL = _BtoW(_DtoB(rtcc->tm_mon + 1),_DtoB(rtcc->tm_mday));
	RTCVAL = _BtoW(_DtoB(rtcc->tm_wday),_DtoB(rtcc->tm_hour));
	RTCVAL = _BtoW(_DtoB(rtcc->tm_min),_DtoB(rtcc->tm_sec));
	
	RCFGCALbits.RTCEN = 1;
	
	RCFGCALbits.RTCWREN = 0;
}
  

/**
 * Reads the actual date/time from the RTCC and put it inside a struct pointer.
 * \param rtcc - a pointer to the struct tm variable to write.
 * \return None
 */
void RTCCGet(struct tm* rtcc)
{	
	RCFGCALbits.RTCPTR = 3;
	
	rtcc->tm_year = _BtoD(RTCVAL) + 100;
	
	buffer = RTCVAL;
	rtcc->tm_mon = _BtoD(buffer>>8) - 1;
	rtcc->tm_mday = _BtoD(buffer);

	buffer = RTCVAL;
	rtcc->tm_wday = _BtoD(buffer>>8);
	rtcc->tm_hour = _BtoD(buffer);

	buffer = RTCVAL;
	rtcc->tm_min = _BtoD(buffer>>8);
	rtcc->tm_sec = _BtoD(buffer);
}

  
/**
 * Function to configure the alarm. It's possible to set the interval for the alarm and if must be repeated continously.
  * \param repeats - Specifies how many time the alarm must be repeated:
 <UL>
	<LI><B>REPEAT_NO</B> - the alarm must not repeated.</LI> 
	<LI><B>an int comprised between 1-254</B> - the number of times alarm must be repeated.</LI> 
	<LI><B>REPEAT_INFINITE</B> - the alarm must be repeated forever.</LI> 
 </UL>
 * \param how often alarm should be raised
 <UL>
	<LI><B>EVERY_HALF_SEC</B> alarm is raised every half second</LI>
	<LI><B>EVERY_SEC</B> alarm is raised every second</LI>
	<LI><B>EVERY_TEN_SEC</B> alarm is raised every 10 seconds</LI>
	<LI><B>EVERY_MIN</B> alarm is raised every minute</LI>
	<LI><B>EVERY_TEN_MIN</B> alarm is raised every 10 minutes</LI>
	<LI><B>EVERY_HOUR</B> alarm is raised every hour</LI>
	<LI><B>EVERY_DAY</B> alarm is raised every day</LI>
	<LI><B>EVERY_WEEK</B> alarm is raised every week</LI>
	<LI><B>EVERY_MONTH</B> alarm is raised every month</LI>
	<LI><B>EVERY_YEAR</B> alarm is raised every year</LI>
 </UL>
 * \param custom user function to execute when alarm event is raised. <B>please use NO_ALRM_EVENT</B> to ignore.
 * \return None
 */
void RTCCAlarmConf(struct tm* rtcc, int repeats, BYTE whenToRaise, void (*fptr)())
{	
	BYTE chime, crepeat;
	if (repeats == 0)
	{
		chime = 0;
		crepeat = 0;
	}
	else if (repeats > 255)
	{
		chime = 1;
		crepeat = 255;
	}
	else 
	{
		chime = 0;
		crepeat = (BYTE)(repeats - 1);
	}
	IEC3bits.RTCIE = 0;
	
	//	Setting function pointer to execute on alarm event
	alarmFP = fptr;
	
	//	Synch operations and critical section to avoid context switch
	vTaskSuspendAll();
	while (RCFGCALbits.RTCSYNC == 1);
	
	//	Enabling writing on RTCC registers
	asm volatile("disi #5");
	__builtin_write_RTCWEN();

	
	ALCFGRPTbits.ALRMEN = 0;

	ALCFGRPTbits.ALRMPTR = 2;
	ALRMVAL = _BtoW(_DtoB(rtcc->tm_mon + 1),_DtoB(rtcc->tm_mday));
	ALRMVAL = _BtoW(_DtoB(rtcc->tm_wday),_DtoB(rtcc->tm_hour));
	ALRMVAL = _BtoW(_DtoB(rtcc->tm_min),_DtoB(rtcc->tm_sec));
	
	
	ALCFGRPTbits.CHIME = chime; 
	ALCFGRPTbits.ARPT = crepeat;
	ALCFGRPTbits.AMASK = whenToRaise;
	
	RCFGCALbits.RTCWREN = 0;
	IPC15bits.RTCIP = 5; 
	IEC3bits.RTCIE = 1; 
	IFS3bits.RTCIF = 0;
	xTaskResumeAll();
}

/**
 * Activates or deactivates the alarm.
  * \param run:
   <UL>
	<LI><B>FALSE</B> Deactivates alarm</LI> 
	<LI><B>TRUE</B> Activates alarm</LI> 
   </UL>
 * \return None
 */
void RTCCAlarmSet(BYTE run) 
{	
	if (run != ALCFGRPTbits.ALRMEN )
	{
		while (RCFGCALbits.RTCSYNC == 1);
		asm volatile("disi #5");
		__builtin_write_RTCWEN();
		ALCFGRPTbits.ALRMEN = run;
		RCFGCALbits.RTCWREN = 0;
	}
}


static WORD _BtoW(BYTE data1, BYTE data2)
{
	WORD word;
	
	word=data1;
	word=word<<8|data2;
	
	return word;
}

//byte to decimal

static BYTE _BtoD(BYTE byte)
{
	BYTE btod = (byte & 0x0F) + 10 * ((byte >> 4) & 0x0F);
	
	return btod;
}

//decimal to byte

static BYTE _DtoB(BYTE dec)
{	
	BYTE dtob = (dec % 10) + (((dec / 10) % 10) << 4);
	
	return dtob;
}
#endif
