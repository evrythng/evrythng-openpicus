/** \file INTlib.c
 *  \brief Hardware library to manage the Interrupts
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
 *  FileName:        INTlib.c
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Simone Marra		1.0		16/08/2011			First Release
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
/// @cond debug
#include "INTlib.h"
#include "p24FJ256GA106.h"

void (*isr_int2)();// = nothigToDo;
void (*isr_int3)();// = nothigToDo;
void (*isr_int4)();// = nothigToDo;

void nothigToDo()
{

}
/// @endcond

/**
 * Detaches the function assigned to the External Interrupt.
 * \param intNum - the number of external interrupt peripheral (2, 3 or 4).
 * \return None
 */
void INTDetach(int intNum)
{
	switch(intNum)
	{
	// Interrupt 2
	case 2:
		isr_int2 = nothigToDo;
		break;
		
	// Interrupt 3
	case 3:
		isr_int3 = nothigToDo;
		break;
		
	// Interrupt 4
	case 4:
		isr_int4 = nothigToDo;;
		break;
	}

}

/**
 * Attaches a generic user function to the External Interrupt.
 * \param intNum - the number of external interrupt peripheral (2, 3 or 4).
 * \param functionName - the name of the user function to execute.
 * \return None
 */
void INTAttach(int intNum, void functionName())
{
	switch(intNum)
	{
	
	// Interrupt 2
	case 2:
		isr_int2 = functionName;
		
		break;
		
	// Interrupt 3
	case 3:
		isr_int3 = functionName;
			
		break;
		
	// Interrupt 4
	case 4:
		isr_int4 = functionName;
	
		break;
	}
}

/**
 * Initialize the External Interrupt.
 * \param intNum - the number of external interrupt peripheral (2, 3 or 4).
 * \param functionName - the name of the user function to execute.
 * \param mode - the edge mode for interrupt execution (0 on positive edge, 1 on negative edge).
 * \return None
 */
void INTInit(int intNum, void functionName(), BOOL mode)
{	
	
	switch(intNum)
	{
	
	// Interrupt 2
	case 2:
		isr_int2 = functionName;
		INTCON2bits.INT2EP = mode;
		IPC7bits.INT2IP = 5;
		break;
		
	// Interrupt 3
	case 3:
		isr_int3 = functionName;
		INTCON2bits.INT3EP = mode;
		IPC13bits.INT3IP = 5;
		break;
		
	// Interrupt 4
	case 4:
		isr_int4 = functionName;
		INTCON2bits.INT4EP = mode;	
		IPC13bits.INT4IP = 5;
		break;
	}
}

/**
 * Changes the External Interrupt Priority.
 * \param intNum - the number of external interrupt peripheral (2, 3 or 4).
 * \param priority - the new priority to use (0->very low, up to 7->very high).
 * \return None
 */
void INTPriority(int intNum, BYTE priority)
{
	switch(intNum)
	{
	
	// Interrupt 2
	case 2:
		IPC7bits.INT2IP = priority;
		break;
		
	// Interrupt 3
	case 3:
		IPC13bits.INT3IP = priority;
		break;
		
	// Interrupt 4
	case 4:
		IPC13bits.INT4IP = priority;	
		break;
	}
}

/**
 * Enables the External Interrupt.
 * \param intNum - the number of external interrupt peripheral (2, 3 or 4).
 * \return None
 */
// Enable external interrupt
void INTEnable(int intNum)
{
	switch(intNum)
	{
	
	// Interrupt 2
	case 2:
		// Reset interrupt 
		IFS1bits.INT2IF = 0;
		IEC1bits.INT2IE = 1;
		break;
		
	// Interrupt 3
	case 3:
		// Reset interrupt 
		IFS3bits.INT3IF = 0; 
		IEC3bits.INT3IE = 1;
		break;
		
	// Interrupt 4
	case 4:
		// Reset interrupt
		IFS3bits.INT4IF = 0;
		IEC3bits.INT4IE = 1;	
		break;
	}
}

/**
 * Disables the External Interrupt.
 * \param intNum - the number of external interrupt peripheral (2, 3 or 4).
 * \return None
 */
// Disable external interrupt
void INTDisable(int intNum)
{
	switch(intNum)
	{
	// Interrupt 2
	case 2:
		IEC1bits.INT2IE = 0;
		break;
		
	// Interrupt 3
	case 3:
		IEC3bits.INT3IE = 0;
		break;
		
	// Interrupt 4
	case 4:
		IEC3bits.INT4IE = 0;	
		break;
	}
}

