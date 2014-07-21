
/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        HWlib.c
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Gabriele Allegria    1.0     1/20/2011		   First release  (core team)
 *  Stefano Cappello 							   Added I2C + support in PWM
 *  Andrea Seraghiti     2.2     11/21/2011        Added ethernet support
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
 
 

#include "HWlib.h"
#include "p24FJ256GA106.h"

extern BOOL TimerOn[5];
extern int __C30_UART;
/****************************************************************************
  Function 		static void HWInit(int conf)

  Description 	This routine initializes the hardware.

  Precondition	None

  Parameters    None - None

  Returns	    None

  Remarks 		None
 ****************************************************************************/
void HWInit(int conf)
{	
	TimerOn[0] = TRUE;
	TimerOn[4] = TRUE;
	__C30_UART = 1;
	if (conf == HWDEFAULT)
	{
// IOs configuration
		IOInit(i1,in);
		IOInit(i2,in);
		IOInit(i3,in);
		IOInit(i4,in);
		IOInit(i5,in);
		
		IOInit(o1,out);
		IOInit(o2,out);
		IOInit(o3,out);
		IOInit(o4,out);
		IOInit(o5,out);

//	ADC initialization	
		ADCInit();
		

	// Unlock registers to PPS configuration
	__builtin_write_OSCCONL(OSCCON & 0xBF);

    #if defined (FLYPORT_WF)
		WF_CS_IO = 1;
		WF_CS_TRIS = 0;

		// PPS configuration
		//Configure SPI1 PPS pins for WiFi
		RPOR10bits.RP21R = 8;					// Assign RP21 to SCK1 (output)
		RPOR13bits.RP26R = 7;					// Assign RP26 to SDO1 (output)
		RPINR20bits.SDI1R = 19;					// Assign RP23 to SDI1 (input)
	
		// Configure INT1 PPS pin 
		RPINR0bits.INT1R = 13;					// Assign RP13 to INT1 (input)
		
        // Configure SPI3 PPS pins for flash
        RPOR10bits.RP20R = 33;                                  // Assign SCK3 to RP20 (output)
        RPOR9bits.RP18R = 32;                                   // Assign SDO3 to RP18 (output)
        RPINR28bits.SDI3R = 28;                                 // Assign RP28 to SDI3 (input)
	
    #elif defined (FLYPORT_ETH)
		// Configure SPI3 PPS pins for flash
        RPOR13bits.RP27R = 33;                                  // Assign SCK3 to RP27 (output)
        RPOR9bits.RP19R = 32;                                   // Assign SDO3 to RP19 (output)
        RPINR28bits.SDI3R = 26;                                 // Assign RP26 to SDI3 (input)
    #endif

		// Configure UART 1 pins
		RPINR18bits.U1RXR = 24;					// Assign RP24 to U1RX (input)
		RPOR11bits.RP22R = 3;					// Assign RP22 to U1TX (output)

			
		// Lock Registers
		__builtin_write_OSCCONL(OSCCON | 0x40);
	}

}

