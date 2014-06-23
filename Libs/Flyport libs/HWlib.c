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
 *  FileName:        HWlib.c
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
/// @cond debug
#include "HWlib.h"
#include "p24FJ256GA106.h"
#if defined (FLYPORTGPRS)
#include "Tick.h"
#else
#include "TCPIP Stack/tick.h"
#endif

void _dbgwrite(char* dbgstr)	
{
	#if defined(STACK_USE_UART) 
		UARTWrite(1, dbgstr);
	#else 
	#endif
}

/*****************************************************************************
*									SECTION:								 *
*	Hardware defines and mapping.											 *
*****************************************************************************/
/*****************************************************************************
 *						--- PORT REGISTER MAPPING ---					 	 *
 ****************************************************************************/	
#define IOPINS	(10)

extern int *LATs[];	
extern int *TRISs[];	
extern int *PORTs[];
extern int *CNPUs[];
extern int *OCCON1s[];
extern int *OCCON2s[];
extern int *OCRs[];
extern int *OCRSs[];
extern int *TCONs[];
extern int *RPORs[];
extern int *RPIRs[];

extern BOOL RPIRPos[];
extern int RPIORPin[];
extern int RPFunc[];
extern int OCM[];
						
extern int *CNPDs[];
extern int IOMode[];

extern int CNPos[];

extern int IOPos[];

extern int an[];

extern int *UMODEs[];
extern int *USTAs[];
extern int *UBRGs[];
extern int *UIFSs[];
extern int *UIECs[];
extern int *UTXREGs[];
extern int *URXREGs[];

extern int UTXIPos[];
extern int URXIPos[];
				
extern int *AD1CFGL;
extern int *AD1CFGH;
extern int *AD1CONF1;
extern int *AD1CONF2;
extern int *AD1CONF3;
extern int *AD1CH;
extern int *AD1CSL;
#if defined (FLYPORTGPRS)
#define MAX_UART_PORTS 3
#else
#define MAX_UART_PORTS 4
#endif				
static int bufind_w[4];
static int bufind_r[4];
static int Status[26];
static char* UartBuffers[UART_PORTS];
static int	 UartSize[UART_PORTS];

#if UART_PORTS >= 1
static char Buffer1[UART_BUFFER_SIZE_1];
#endif
#if UART_PORTS >= 2
static char Buffer2[UART_BUFFER_SIZE_2];
#endif
#if UART_PORTS >= 3
static char Buffer3[UART_BUFFER_SIZE_3];
#endif
#if UART_PORTS >= 4
static char Buffer4[UART_BUFFER_SIZE_4];
#endif


static char buffover [MAX_UART_PORTS];
static BYTE last_op[MAX_UART_PORTS];

static int OCTimer[9];
static char OCTSel[9];
BOOL TimerOn[5];

static unsigned int period;
/// @endcond



/**
\defgroup ADC
@{
The ADC library contains the command to manage the ADC, so it's possible to easy convert an analog voltage value. 
The precision of the ADC is 10 bit.

*/

/**
 * Initializes the ADC module with default values. <B>NOTE:</B> this function is already called at Flyport startup.
 * \param None
 * \return None
 */
void ADCInit()
{	
	//	AD1CON1 settings:
	//	ADC OFF, continue operation in idle mode, output like integer, autoconv.
	AD1CON1 = 0x00E0;	
	
	//	AD1CON2 settings:
	//	External Vref, input scan, 2 sequences per interrupt, buffer is 16bit wide
	//	only MUX A used.
	AD1CON2 = 0x6000;
	
	//	AD1CON3 settings:
	//	Clock derived from system clock, auto sample: 1Tad, conv. clock = 5 * Tcy
	AD1CON3 = 0x0F10;
	
	AD1PCFGL = ADCCHANNELSL;		//	ADC channels enabled
	AD1PCFGH = 0x3;					//	Internal band gap disabled for scan

	//	AD1CHS settings:
	//	MUX A inputs Vref+ and Vref-.
	AD1CHS = 0x0; 
	AD1CSSL = 0x0;	
	
	//IPC3bits.AD1IP = 6;
	IFS0bits.AD1IF  = 0;
	IEC0bits.AD1IE  = 1;
	AD1CON1bits.ADON = 1;
}

static int AD_val = 0;
static BOOL AD_flag = FALSE;

void __attribute__ ((__interrupt__, no_auto_psv)) _ADC1Interrupt(void)
{
	IFS0bits.AD1IF = 0;	
	AD_val = ADC1BUF0;
	AD_flag = TRUE;
}
 /**
 * Reads the value of the analog channel specified. 
 * \param ch The number of the analog channel to read. For the number of the channel, refer to the Flyport pinout.
 * \return An int containing the value read by the function. The value is comprise between 0 and 1023 (2.048V).
 */
int ADCVal(int ch)
{
	AD1CHS = an[ch];
	// Added to stability correction 
	// on using ADCVal with webserver active

 	IFS0bits.AD1IF = 0;	
	AD_flag = FALSE;
	AD1CON1bits.SAMP = 1;
	while (!AD_flag);
	return AD_val;
}

/**
* Enables selected analog channel to the related pin (in this way it cannot be used as digital IO pin)
* \param ch - channel 
*/
void ADCAttach(int ch)
{
	//	ADC OFF
	AD1CON1bits.ADON = 0;
	
	// Read old value
	unsigned int adNewVal = AD1PCFGL;
	
	// generate bit mask for AND operation
	unsigned int unsetVal = (1<<an[ch]);
	unsetVal = ~unsetVal;
	
	// Add selected channel (clear related bit on register) 
	adNewVal = (unsetVal & adNewVal) & 0xFFFF;
	
	AD1PCFGL = adNewVal;		//	update 	ADC channels enabled
	AD1PCFGH = 0x3;				//

	// ADC ON
	AD1CON1bits.ADON = 1;
}

/**
* Disables selected analog channel to the related pin (in this way it can be used as digital IO pin)
* \param ch - channel 
*/
void ADCDetach(int ch)
{
	//	ADC OFF
	AD1CON1bits.ADON = 0;
	
	// Read old value
	unsigned int adNewVal = AD1PCFGL;
	
	// Remove selected channel (set related bit on register)
	adNewVal |= (1<<an[ch]);
	
	AD1PCFGL = adNewVal;		//	update 	ADC channels enabled
	AD1PCFGH = 0x3;				//

	// ADC ON
	AD1CON1bits.ADON = 1;	
}

/*! @} */


/**
\defgroup GPIOs
@{

The GPIOs commands can be used the manage the IO pins, to configure, to change or to read their values.
*/

/**
 * Puts the putval value on the specified IO output pin. 
 * \param io Specifies the ouput pin.
 * \param putval The value to assign to the pin:
 <UL>
	<LI><B>on (or ON, or 1)</B> high level (about 3.3V).</LI>
	<LI><B>off (or OFF, or 0)</B> low level (0V).</LI>	
	<LI><B>toggle (or TOGGLE, or 2)</B> toggles between the actual value of the pin and the inverse.</LI>
 </UL>
 * \return None
 */
void IOPut(int io, int putval)
{	
	io--;
	WORD addval = 0;
	switch(putval)
	{
	//	Output clear
	case 0:
		addval = 1 << IOPos[io];
		addval = ~addval;
		*LATs[io] = *LATs[io] & addval;
		Status[io] = 0; 
		break;
		
	//	Output set
	case 1:
		addval = 1 << IOPos[io];
		*LATs[io] = *LATs[io] | addval;
		Status[io] = 1;
		break;
		
	//	Output toggle
	case 2:
		if (Status[io] == 1)
		{
			addval = 1 << IOPos[io];
			addval = ~addval;
			*LATs[io] = *LATs[io] & addval;	
			Status[io] = 0;
		}
		else
		{
			addval = 1 << IOPos[io];
			*LATs[io] = *LATs[io] | addval;	
			Status[io] = 1;
		}
		break;
	}
}


/**
 * Initializes the specified pin with the desired function. Remappable pins allows the user to set the pin not only like input or output, but also with advanced functionalities, like UART or SPI.
 * \param io Specifies the pin.
 * \param putval Specifies how the pin must be initialized. The valid parameters are the following:
 <UL>
	<LI><B>in (or IN)</B> input pin.</LI>
	<LI><B>inup (or INUP)</B> input pin with pullup resistor (about 5 KOhm).</LI>	
	<LI><B>indown (or INDOWN)</B> input pin with pulldown resistor (about 5 Kohm).</LI>
	<LI><B>out (or OUT)</B> output pin.</LI>
	<LI><B>UART1RX</B> UART1 RX input pin.</LI>
	<LI><B>UART1CTS</B> UART1 CTS input pin.</LI>
	<LI><B>UART2RX</B> UART2 RX input pin.</LI>
	<LI><B>UART2CTS</B> UART2 CTS input pin.</LI>
	<LI><B>UART3RX</B> UART3 RX input pin.</LI>
	<LI><B>UART3CTS</B> UART3 CTS input pin.</LI>
	<LI><B>UART4RX</B> UART4 RX input pin. <B><I>Note:</B>not available for Flyport GPRS</I></LI>
	<LI><B>UART4CTS</B> UART4 CTS input pin. <B><I>Note:</B>not available for Flyport GPRS</I></LI>
	<LI><B>EXT_INT2</B> External Interrupt 2 input pin.</LI>
	<LI><B>EXT_INT3</B> External Interrupt 3 input pin.</LI>
	<LI><B>EXT_INT4</B> External Interrupt 4 input pin.</LI>
	<LI><B>SPICLKIN</B> SPI clock input pin (only in slave mode).</LI>
	<LI><B>SPI_IN</B> SPI data input pin.</LI>
	<LI><B>SPI_SS</B> SPI slave select input pin (only in slave mode).</LI>
	<LI><B>TIM_4_CLK</B> External Timer 4 input pin.</LI>
	<LI><B>UART1TX</B> UART1 TX output pin.</LI>
	<LI><B>UART1RTS</B> UART1 RTS output pin.</LI>
	<LI><B>UART2TX</B> UART2 TX output pin.</LI>
	<LI><B>UART2RTS</B> UART2 RTS output pin.</LI>
	<LI><B>UART3TX</B> UART3 TX output pin.</LI>
	<LI><B>UART3RTS</B> UART3 RTS output pin.</LI>
	<LI><B>UART4TX</B> UART4 TX output pin. <B><I>Note:</B>not available for Flyport GPRS</I></LI>
	<LI><B>UART4RTS</B> UART4 RTS output pin. <B><I>Note:</B>not available for Flyport GPRS</I></LI>
	<LI><B>SPICLKOUT</B> SPI clock output pin (only in master mode).</LI>
	<LI><B>SPI_OUT</B> SPI data output pin.</LI>
	<LI><B>SPI_SS_OUT</B> SPI slave select output pin (only in master mode).</LI>
	<LI><B>RESET_PPS</B> Removes previously selected PPS output function.</LI>
</UL>
 * \return None
 */ 
void IOInit(int io, int putval) 
{
	io--;
	WORD addval = 0;
	if (putval < 5)
	{
		switch (putval)
		{
		//	Pin set as OUPUT first disables pull-up and pull-down, then set pin 
		//	as output
		case 0:
			addval = 1 << CNPos[io];
			addval = ~addval;
			*CNPDs[io] = *CNPDs[io] & addval;
			*CNPUs[io] = *CNPUs[io] & addval;
			addval = 0;
			addval = 1 << IOPos[io];
			addval = ~addval;
			*TRISs[io] = *TRISs[io] & addval;
			IOMode[io] = 0;
			break;
			
		//	Pin set as INPUT - first sets the pin as input, then disables pull-up 
		//	and pull-down
		case 1:
			addval = 1 << IOPos[io];
			*TRISs[io] = *TRISs[io] | addval;	
			addval = 0;	
			addval = 1 << CNPos[io];
			addval = ~addval;
			*CNPDs[io] = *CNPDs[io] & addval;
			*CNPUs[io] = *CNPUs[io] & addval;	
			IOMode[io] = 1;
			break;
			
		//	Pin set as INPUT with Pull-up - First disables pull-down, then sets 
		//	pin as input and enables pull-up
		case 2:
			addval = 1 << CNPos[io];
			addval = ~addval;
			*CNPDs[io] = *CNPDs[io] & addval;
			addval = ~addval;
			*CNPUs[io] = *CNPUs[io] | addval;
			addval = 0;
			addval = 1 << IOPos[io];
			*TRISs[io] = *TRISs[io] | addval;
			IOMode[io] = 2;
			break;
			
		//	Pin set as INPUT with Pull-down - First disables pull-up, then sets
		//	pin as input and enables pull-down
		case 3:
			addval = 1 << CNPos[io];
			addval = ~addval;
			*CNPUs[io] = *CNPUs[io] & addval;
			addval = ~addval;
			*CNPDs[io] = *CNPDs[io] | addval;	
			addval = 0;
			addval = 1 << IOPos[io];
			*TRISs[io] = *TRISs[io] | addval;
			IOMode[io] = 3;		
		}
	}
	else if (putval < 31)
	{
		if (RPIORPin[io] != 0)
		{
			if ( (putval < 13) || (putval > 15) )
			{
				addval = 1 << IOPos[io];
				*TRISs[io] = *TRISs[io] | addval;	
				addval = 0;	
				addval = 1 << CNPos[io];
				addval = ~addval;
				*CNPDs[io] = *CNPDs[io] & addval;
				*CNPUs[io] = *CNPUs[io] & addval;	
				IOMode[io] = 1;
			}
			
			int rpdummy;
			if (RPIORPin[io] < 0)
				rpdummy = -RPIORPin[io];
			else 
				rpdummy = RPIORPin[io];
			int reg_val;
			if (RPIRPos[putval-5])
			{
				reg_val = 0x00FF & (*RPIRs[putval-5]);
				reg_val = reg_val | ( rpdummy << 8);
			}
			else
			{
				reg_val = 0xFF00 & (*RPIRs[putval-5]);
				reg_val = reg_val | rpdummy;
			}
			__builtin_write_OSCCONL(OSCCON & 0xBF);						// Unlock registers	
			(*RPIRs[putval-5]) = reg_val;
			__builtin_write_OSCCONL(OSCCON | 0x40);						// Lock register
		}
	}
	else if (putval == RESET_PPS)
	{
		__builtin_write_OSCCONL(OSCCON & 0xBF);							// Unlock registers
		if (RPIORPin[io]%2==0)
		{					  											// Tests which pin is remapped
			*RPORs[io] =  (*RPORs[io]&0xFFC0); 							// Write on RPOR from 0 to 5th bit
		}
		else
		{
			*RPORs[io] = (*RPORs[io]&0xC0FF);							// Write on RPOR from 8th to 13th bit
		}
		__builtin_write_OSCCONL(OSCCON | 0x40);	
	}
	else if (putval > 30)
	{
		__builtin_write_OSCCONL(OSCCON & 0xBF);							// Unlock registers
		if (RPIORPin[io]%2==0)
		{					  											// Tests which pin is remapped
			*RPORs[io] =  (*RPORs[io]&0x3F00)|(RPFunc[putval-31]);		// Write on RPOR from 0 to 5th bit
		}
		else
		{
			*RPORs[io] = (*RPORs[io]&0x3F)|(RPFunc[putval-31]<<8);		// Write on RPOR from 8th to 13th bit
		}
		__builtin_write_OSCCONL(OSCCON | 0x40);	
	}

}		


 /**
 * Reads the value of the specified pin. 
 * \param io The pin to read.
 * \return An int with the value read by the function.
 */
int IOGet(int io)
{
	io--;
	WORD addval = 0;
	if (IOMode[io] == 0)
	{
		int resget;
		resget = Status[io];

		return resget;
	}
	addval = 1 << IOPos[io];
	addval = addval & *PORTs[io];
	if (addval>0)
	{
		Status[io] = 1;
		return 1;		
	}
	else
	{
		Status[io] = 0;
		return 0;
	}

	return -1;
}


 /**
 * IOButtonState - Polls for the state of the button implemented on the specified pin. This command doesn't return the voltage level of the pin,
 * but if the button has been pressed or released. No problem with the "debounce" of the button. It doesn't matter if the button is implemented with a 
 * "low logic" or "high logic". You just have to initialize the pin like "inup" or "indown". 
 * \param io The pin to read.
 * \return 
  <UL>
	<LI><B>pressed:</B> if the button has been pressed.</LI>
	<LI><B>released:</B> if the button has been released.</LI>
  </UL>
 */
static DWORD tdebounce1 = 0;

int IOButtonState(int io)
{
    DWORD tdebounce2;
	tdebounce2 = TickGetDiv256();
	if ((tdebounce2-tdebounce1)>5)
	{
		io = io - 1;
		int stat;
		stat = Status [io];
		
		if (IOGet(io+1) > stat)
		{
			if (IOMode[io] == 2)	
			{
				tdebounce1 = TickGetDiv256();
				return 1;	
			}
			else		
			{
				tdebounce1 = TickGetDiv256();
				return 2;	
			}
		}

		if (IOGet(io+1) < stat)
		{
			if (IOMode[io] == 2)	
			{
				tdebounce1 = TickGetDiv256();	
				return 2;	
			}
			else
			{
				tdebounce1 = TickGetDiv256();
				return 1;	
			}
		}
		if (stat == Status[io])
			return 0;	
		return -1;
	}
	return 0;	
}	


#if (defined (FLYPORT_G) || defined(FLYPORT_LITE))
/**
 * Set Green Led on/off/toggle.<B>NOTE:</B> Flyport Wifi-G and LITE Only!!
 \param val The value to assign to the Green Led:
 <UL>
	<LI><B>on (or ON, or 1)</B> turns ON led.</LI>
	<LI><B>off (or OFF, or 0)</B> turns OFF led.</LI>	
	<LI><B>toggle (or TOGGLE, or 2)</B> toggles between the actual status of the LED and the inverse.</LI>
 </UL>
 * \return None
 */
void PowerLed(int val)
{
	TRISEbits.TRISE4 = 0;
	if(val == off)
		LATEbits.LATE4 = 0;
	else if(val == on)
		LATEbits.LATE4 = 1;
	else if(val == toggle)
		LATEbits.LATE4 = !LATEbits.LATE4;
}
#endif


/*! @} */


/**
\defgroup UART
@{
The UART section provides serial communication. The flyport implements a buffer of 256 characters for the UART, to make serial communicate easier.
*/


 /**
 * Initializes the specified uart port with the specified baud rate.
 * \param port - the UART port to initialize. <B><I>Note:</B> port 4 not available for Flyport GPRS</I>
 * \param baud - the desired baudrate.
 * \return None
 */
void UARTInit(int port,long int baud)
{
	#if defined (FLYPORTGPRS)
	if(port < 4)
	{
	#endif
		port--;
		switch(port)
		{
			#if UART_PORTS >= 1
			case 0:
				UartSize[port] = UART_BUFFER_SIZE_1;
				UartBuffers[port] = Buffer1;
				break;
			#endif
			
			#if UART_PORTS >= 2
			case 1:
				UartSize[port] = UART_BUFFER_SIZE_2;
				UartBuffers[port] = Buffer2;
				break;
			#endif
			
			#if UART_PORTS >= 3
			case 2:
				UartSize[port] = UART_BUFFER_SIZE_3;
				UartBuffers[port] = Buffer3;
				break;
			#endif
			
			#if UART_PORTS >= 4
			case 3:
				UartSize[port] = UART_BUFFER_SIZE_4;
				UartBuffers[port] = Buffer4;
				break;
			#endif
			
			default:
				break;
		}
		long int brg , baudcalc , clk , err;
		clk = GetInstructionClock();
		brg = (clk/(baud*16ul))-1;
		baudcalc = (clk/16ul/(brg+1));
		err = (abs(baudcalc-baud)*100ul)/baud;

		if (err<2)
		{
			*UMODEs[port] = 0;
			*UBRGs[port] = brg;
		}
		else
		{
			brg = (clk/(baud*4ul))-1;
			*UMODEs[port] = 0x8;
			*UBRGs[port] = brg;
		}
	#if defined (FLYPORTGPRS)
	}
	#endif
}


 /**
 * After the initialization, the UART must be turned on with this command.
 * \param port - the UART port to turn on. <B><I>Note:</B> port 4 not available for Flyport GPRS</I>
 * \return None
 */
void UARTOn(int port)
{
	#if defined (FLYPORTGPRS)
	if(port < 4)
	{
	#endif
		port--;
		*UMODEs[port] = *UMODEs[port] | 0x8000;
		*USTAs[port] = *USTAs[port] | 0x400;

		*UIFSs[port] = *UIFSs[port] & (~URXIPos[port]);
		*UIFSs[port] = *UIFSs[port] & (~UTXIPos[port]);
		*UIECs[port] = *UIECs[port] | URXIPos[port];
		bufind_w[port] = 0;
		bufind_r[port] = 0;
		last_op[port] = 0;
	#if defined (FLYPORTGPRS)
	}
	#endif
}


 /**
 * Turns off the specified UART port.
 * \param port - the UART port to turn off. <B><I>Note:</B> port 4 not available for Flyport GPRS</I>
 * \return None
 */
void UARTOff(int port)
{
	#if defined (FLYPORTGPRS)
	if(port < 4)
	{
	#endif
		port--;
		*USTAs[port] = *USTAs[port] & 0xFBFF;
		*UMODEs[port] = *UMODEs[port] & 0x7FFF;
		
		*UIECs[port] = *UIECs[port] & (~URXIPos[port]);
		*UIECs[port] = *UIECs[port] & (~UTXIPos[port]);
	#if defined (FLYPORTGPRS)
	}
	#endif
}

/// @cond debug
/*---------------------------------------------------------------------------- 
  |	Function: 		UARTRxInt(int port)		 								 |
  | Description: 	Specific funtion to read the UART from inside the ISR.	 |
  |					It fills the static array Buffer[]						 |
  | Returns:		-														 |
  | Parameters:		int port - specifies the port (1 to 4)					 |
  --------------------------------------------------------------------------*/
void UARTRxInt(int port)
{
	port--;
	while ((*USTAs[port] & 1)!=0)
	{
		if (bufind_w[port] == bufind_r[port])
		{
			if (last_op[port] == 1)
			{
				buffover[port] = 1;	
				bufind_w[port] = 0;
				bufind_r[port] = 0;
				last_op[port] = 0;
			}
		}	
		*(UartBuffers[port]+bufind_w[port]) = *URXREGs[port];
		
		if (bufind_w[port] == UartSize[port]- 1)
		{
			bufind_w[port] = 0;
		}
		else
			bufind_w[port]++;
	}
	last_op[port] = 1;	
	*UIFSs[port] = *UIFSs[port] & (~URXIPos[port]);
}
/// @endcond



 /**
 * Flushes the buffer of the specified UART port.
 * \param port - the UART port to flush. <B><I>Note:</B> port 4 not available for Flyport GPRS</I>
 * \return None
 */
void UARTFlush(int port)
{
	#if defined (FLYPORTGPRS)
	if(port < 4)
	{
	#endif
		port = port-1;
		bufind_w[port] = 0;
		bufind_r[port] = 0;
		last_op[port] = 0;
	#if defined (FLYPORTGPRS)
	}
	#endif
}


 /**
 * Returns the RX buffer size of the specified UART port.
 * \param port - the UART port to read. <B><I>Note:</B> port 4 not available for Flyport GPRS</I>
 * \return the number of characters that can be read from the specified serial port.
 */
int UARTBufferSize(int port)
{
	#if defined (FLYPORTGPRS)
	if(port < 4)
	{
	#endif
		port = port-1;
		BYTE loc_last_op = last_op[port];
		int conf_buff;
		int bsize=0;

		conf_buff = bufind_r[port] - bufind_w[port];
		if (conf_buff > 0)
			bsize = UartSize[port] - bufind_r[port] + bufind_w[port];
		else if (conf_buff < 0)

			bsize = bufind_w[port] - bufind_r[port];
		else if (conf_buff == 0)
			if (loc_last_op == 1)
				bsize = UartSize[port];

		return bsize;
	#if defined (FLYPORTGPRS)
	}
	else
		return -1;
	#endif
}


 /**
 * Reads characters from the UART RX buffer and put them in the char pointer "towrite" . Also returns the report for the operation.
 * \param port - the UART port to read. <B><I>Note:</B> port 4 not available for Flyport GPRS</I>
 * \param towrite - the char pointer to fill with the read characters.
 * \param count - the number of characters to read
 * \return the report for the operation:
  <UL>
	<LI><B>n>0:</B> N characters correctly read.</LI> 
	<LI><B>n<0:</B> N characters read, but buffer overflow detected.</LI> 
 </UL>
 */
int UARTRead(int port , char *towrite , int count)
{
	#if defined (FLYPORTGPRS)
	if(port < 4)
	{
	#endif
		int rd,limit;
		limit = UARTBufferSize(port);
		if (count > limit)
			count=limit;
		port = port-1;
		int irx = 0;
		rd = 0;
		while (irx < count)
		{
			*(towrite+irx) = *(UartBuffers[port]+bufind_r[port]);

			if (bufind_r[port] == UartSize[port]-1)
				bufind_r[port] = 0;
			else
				bufind_r[port]++;		

			irx++;
		}
		
		if ( buffover [port] != 0 )
		{
			rd = -count;
			buffover[port] = 0;
		}
		else
			rd = count;

		last_op[port] = 2;
		return rd;
	#if defined (FLYPORTGPRS)
	}
	else
		return -1;
	 #endif
}



 /**
 * Writes the specified string on the UART port.
 * \param port - the UART port to write to. <B><I>Note:</B> port 4 not available for Flyport GPRS</I>
 * \param buffer - the string to write (a NULL terminated char array).
 * \return None
 */
void UARTWrite(int port, char *buffer)
{
	#if defined (FLYPORTGPRS)
	if(port < 4)
	{
	#endif
		port--;
		int pdsel;
		// transmits till NUL character is encountered 
		pdsel = (*UMODEs[port] & 6) >>1;
		if (pdsel == 3)                             // checks if TX is 8bits or 9bits
		{
			while(*buffer != '\0') 
			{
				while((*USTAs[port] & 512)>0);	// waits if the buffer is full 
				*UTXREGs[port] = *buffer++;         // sends char to TX reg
			}
		}
		else
		{
			while(*buffer != '\0')
			{
				while((*USTAs[port] & 512)>0);      // sends char to TX reg
				*UTXREGs[port] = *buffer++ & 0xFF;  // sends char to TX reg
			}
		}
	#if defined (FLYPORTGPRS)
	}
	#endif
}


 /**
 * Writes a single character on the UART port.
 * \param port - the UART port to write to. <B><I>Note:</B> port 4 not available for Flyport GPRS</I>
 * \param char - the char to write.
 * \return None
 */
void UARTWriteCh(int port, char chr)
{
	#if defined (FLYPORTGPRS)
	if(port < 4)
	{
	#endif
		port--;
		int pdsel;
		pdsel = (*UMODEs[port] & 6) >>1;
		if(pdsel == 3)        /* checks if TX is 8bits or 9bits */
		{
			while((*USTAs[port] & 512)>0);	/* waits if the buffer is full */
			*UTXREGs[port] = chr;    		/* transfer data to TX reg */
		}
		else
		{
			while((*USTAs[port] & 512)>0); /* waits if the buffer is full */
			*UTXREGs[port] = chr & 0xFF;   /* transfer data to TX reg */
		}
	#if defined (FLYPORTGPRS)
	}
	#endif
}

/*! @} */



/**
\defgroup PWM
@{
With the PWM library is possible to easily manage up to nine different PWM (different frequency and duty cycle). To use the PWM on a pin you first have to initialize the PWM, 
then assign it to the desired pin. At runtime it' possible to change the duty cycle of the PWM.
<BR><B>NOTE</B> :it possible to assign a PWM to any pin, also to the input pins.
*/


 /**
 * Initializes the specified PWM with the desired frequency and duty cycle .
 * \param freq Frequency in hertz.
 * \param dutyc Ducty cycle for the PWM in percent (0-100).
 * \return None
 */
void PWMInit(BYTE pwm, float freq, float dutyc)
{
	float tpwm;
	float tcy= 0.0000000625;
	tpwm = 1 / freq;
	unsigned int d;
	float dcent = dutyc/100;

	pwm = pwm - 1;

	*OCCON1s[pwm] = 0;								// Reset OC1CON1 Register	
	*OCCON2s[pwm] = 0;								// Reset OC1CON2 Register

	if (freq > 244)
	{
		//	Timer 2 selected: prescaler 0
		period = (unsigned int)(tpwm/(tcy))-1;
		d = (unsigned)(dcent*(period +1));
		*TCONs[1] = *TCONs[1] & 0xFFCF;

		OCTSel[pwm] = 0;
		OCTimer[pwm] = 1;
	}
	else if (freq>30)
	{
		//	Timer 5 selected: prescaler 1
		period = (unsigned int)(tpwm/(tcy*8))-1;
		d = (unsigned)(dcent*(period +1));
		OCTSel[pwm] = 3;
		OCTimer[pwm] = 4;
	}
	else if (freq>3)
	{
		//	Timer 3 selected: prescaler 2
		period = (unsigned int)(tpwm/(tcy*64))-1;
		d = (unsigned)(dcent*(period +1));	
		*TCONs[2] = (*TCONs[2] & 0xFFCF) | 0x20;
		OCTSel[pwm] = 1;
		OCTimer[pwm] = 2;
	}
	else
	{
		//	Timer 1 selected: prescaler 3
		period = (unsigned int)(tpwm/(tcy*256))-1;
		d = (unsigned int)(dcent*(period +1));	
		OCTSel[pwm] = 4;
		OCTimer[pwm] = 0;
	}

	*OCRs[pwm] = d;						// Set duty-cycle
	*OCRSs[pwm] = period;

}


 /**
 * Turns on the specified PWM on the specified pin.
 * \param io Pin to assign the PWM.
 * \param pwm PWM number previously defined in PWMInit.
 * \return None
 */
void PWMOn(BYTE io, BYTE pwm )
{	
	io--;
	pwm = pwm -1;
	if (RPIORPin[io] > 0)
	{
		__builtin_write_OSCCONL(OSCCON & 0xBF);					// Unlock registers
		if (RPIORPin[io]%2==0)
		{					  									// Tests which pin is remapped
			*RPORs[io] =  (*RPORs[io]&0x3F00)|(OCM[pwm]);		// Write on RPOR from 0 to 5th bit
		}
		else
		{
			*RPORs[io] = (*RPORs[io]&0x3F)|(OCM[pwm]<<8);		// Write on RPOR from 8th to 13th bit
		}
		__builtin_write_OSCCONL(OSCCON | 0x40);
		
		*OCCON2s[pwm] = 0x1F;									// SYNCSEL = Ox1F ( Source of Syncronism), OCTRIG=0
		*OCCON1s[pwm] = 0x0006 | (OCTSel[pwm]<<10);				// Start PWM
		if (TimerOn[OCTimer[pwm]] == FALSE)
		{
			*TCONs[OCTimer[pwm]] = *TCONs[OCTimer[pwm]] | 0x8000;
			TimerOn[OCTimer[pwm]] = TRUE;
		}	
	}
}


 /**
 * Changes the duty cycle of the PWM without turning it off. Useful for motors or dimmers.
 * \param duty New duty cycle desired (0-100).
 * \param pwm PWM number previously defined in PWMInit.
 * \return None
 */
void PWMDuty(float duty, BYTE pwm)
{
	pwm--;
	unsigned calcpwm, respwm;
	float calcpwm2;
	
	calcpwm = *OCRSs[pwm];
	calcpwm2 = (calcpwm + 1) / 100;
	respwm = calcpwm2 * duty;
	*OCRs[pwm] =(unsigned int) respwm; 
}

 /**
 * Turns off the specified PWM.
 * \param pwm PWM number previously defined in PWMInit.
 * \return None
 */
void PWMOff(BYTE pwm)
{
	pwm--;
	*OCCON1s[pwm] = 0x0000;					// Stop PWM	
}
/*! @} */


/**
\defgroup I2C
@{
The I2C library allows the user to communicate with external devices with I2C bus, like flash memories or sensors. The Flyport is initialized as I2C master.
*/

///@cond debug
static BOOL _i2cTimeout = FALSE;
static BYTE _i2cAddrSize = 1;
///@endcond

 /**
 * Initializes the I2C module.
 * \param I2CSpeed The required speed for the I2C module,  the possible defines are:
  <UL>
	<LI><B>HIGH_SPEED:</B> I2C high speed (400KHz).</LI> 
	<LI><B>LOW_SPEED</B> I2C low speed (100 KHz).</LI> 
 </UL> 
 * \return None
 */
void I2CInit(BYTE I2CSpeed)
{
	TRISGbits.TRISG2 = 1;
	TRISGbits.TRISG3 = 1;

	I2C1TRN = 0x0000;
	I2C1RCV = 0x0000;
		

	I2C1BRG = I2CSpeed;			// Set I2C module at I2CSpeed (100KHz or 400KHz)
	I2C1CON = 0x8200;			// Configuration of module
}


 /**
 * Sends a start sequence on the bus. The operation has a timeout of 0.5 secs.
 * \param None
 * \return A BOOL value reporting the result for the operation.
 */
BOOL I2CStart()
{
	// Check if I2C1 module is enabled..
	if(I2C1CONbits.I2CEN == 0)
		return FALSE;
	
	unsigned int i2cCount = 0;
	I2C1CONbits.SEN = 1;		 		// Sends a start sequence on I2C bus	
	while(I2C1CONbits.SEN)				// waits the end of start
	{
		i2cCount++;
		Delay10us(1);
		if (i2cCount == 50000)
			return FALSE;
	}
	return TRUE;
}


 /**
 * Sends a repeated start sequence on the bus. The operation has a timeout of 0.5 secs.
 * \param None
 * \return A BOOL value reporting the result for the operation.
 */
BOOL I2CRestart()
{
	// Check if I2C1 module is enabled..
	if(I2C1CONbits.I2CEN == 0)
		return FALSE;
	
	unsigned int i2cCount = 0;
	I2C1CONbits.RSEN=1;				// Sends a repeated start sequence
	while(I2C1CONbits.RSEN)		// waits the end of restart
	{
		i2cCount++;
		Delay10us(1);
		if (i2cCount == 50000)
			return FALSE;
	}
	return TRUE;
}


 /**
 * Stops the trasmissions on the bus. The operation has a timeout of 0.5 secs.
 * \param None
 * \return A BOOL value reporting the result for the operation.
 */
BOOL I2CStop()
{
	// Check if I2C1 module is enabled..
	if(I2C1CONbits.I2CEN == 0)
		return FALSE;
	
	unsigned int i2cCount = 0;
	I2C1CONbits.PEN=1;				// Initiate a Stop condition on the bus
	while(I2C1CONbits.PEN)			// waits the end of stop
	{

		i2cCount++;
		Delay10us(1);
		if (i2cCount == 50000)
			return FALSE;
	}
	return TRUE;
}

/**
 * Executes a polling on device address to check if I2C device is ready or busy/not responding
 *	\return	TRUE I2C device is ready
 *	\return FALSE I2C device is busy/not responding
 */
BOOL I2CGetDevAck(BYTE devAddress)
{
	// Check if I2C1 module is enabled..
	if(I2C1CONbits.I2CEN == 0)
		return FALSE;
	
	BOOL result = FALSE;
	vTaskSuspendAll();
	I2CStart();										// Start sequence
	result = I2CWrite((devAddress << 1) & 0xFE);	// Poll device ID Address

	I2CStop();							
	xTaskResumeAll();
			
	return result;
}

/**
 * Return TRUE if last I2C Read failed for a timeout error. 
 * Calling this function last value is cleared
 * \return a BOOL value reporting if a timeout occurred during last I2C operation.
 */
BOOL I2CTimeout()
{
	BOOL currValue = _i2cTimeout;
	_i2cTimeout = FALSE;
	return currValue;
}

/**
 * Set register address size
 * \param addrSize 1 to use a 1 BYTE register address size
 * \param addrSize 2 to use a 2 BYTEs register address size
 */
void I2CAddrSizeSet(BYTE addrSize)
{
	if(addrSize == 1)	// 1 BYTE address size
		_i2cAddrSize = addrSize;
	else if(addrSize == 2)	// 2 BYTEs address size
		_i2cAddrSize = addrSize;
	else
		_i2cAddrSize = 1; // Default condition: 1 BYTE address size
}

/**
 * Reads current register address size 
 * \return 1 for 1 BYTE register address size
 * \return 2 for 2 BYTEs register address size
 */
BYTE I2CAddrSizeGet()
{
	return _i2cAddrSize;
}

 /**
 * Writes one byte on the I2C bus.
 * \param data The Byte to write on I2C.
 * \return TRUE received a ACK Acknowledge from device
 * \return FALSE received a NACK Acknowledge from device
 */
BOOL I2CWrite(BYTE data)
{
	// Check if I2C1 module is enabled..
	if(I2C1CONbits.I2CEN == 0)
		return FALSE;
		
	unsigned int cnt = 0;
	BOOL devAck = FALSE;
	
	vTaskSuspendAll();
	
	I2C1TRN = data;					// Sends a byte on the bus

	// Wait for the end of trasmission or timeout
	while(cnt < 50000)
	{
		if(I2C1STATbits.TRSTAT || I2C1STATbits.TBF)		// waits the end of trasmissions
		{
			cnt++;
			Delay10us(1);
		}
		else
		{
			cnt = 0;
			Delay10us(1);
			if(I2C1STATbits.ACKSTAT == 0)
				devAck = TRUE;
			else
				devAck = FALSE;
			break;
		}	
	}
	xTaskResumeAll();
	return devAck;
	
} 


 /**
 * Reads one byte from the data bus.
 * \param ack the value of the Acknowledge to send (0 or 1)
 * \return the value read by the function.
 */
BYTE I2CRead(BYTE ack)
{
	// Check if I2C1 module is enabled..
	if(I2C1CONbits.I2CEN == 0)
		return FALSE;
		
	unsigned int cnt = 0;
	vTaskSuspendAll();
	I2C1CONbits.RCEN=1;	 // Reads one byte from the bus
	Delay10us(5);

	_i2cTimeout = FALSE;
	// waits the end of the read, 5secs timeout
	while(I2C1CONbits.RCEN)	
	{
		cnt++;
		Delay10us(10);
		if (cnt == 50000)
		{
			_i2cTimeout = TRUE;
			xTaskResumeAll();
			return 0;
		}
	}

	I2C1STATbits.I2COV = 0;	 // Resets the overflow flag

	I2C1CONbits.ACKDT = ack; 
	I2C1CONbits.ACKEN = 1;	 // Initiate an aknowledge sequence
	cnt = 0;
	// wait the end of the acknowledge sequence, 2 secs timeout
	while(I2C1CONbits.ACKEN)	
	{
		cnt++;
		Delay10us(10);
		if (cnt == 20000)
		{
			_i2cTimeout = TRUE;
			xTaskResumeAll();
			return 0;
		}
	}	
	xTaskResumeAll();
	return I2C1RCV;	 // Returns the byte
}


 /**
 * Reads the value of one register from the I2C device with the specified address.
 * \param devAddr The address of the device to read. The address is expressed in 7 bit format.
 * \param regAddr The address of the register to read. 
 * \param rwDelay The delay (expressed in 10us) between the write and the read instruction, since some I2C device requires it.
 * \return the BYTE read with the operation.
 */
char I2CReadReg(BYTE devAddr, unsigned int regAddr, unsigned int rwDelay)
{
	unsigned char v;
	devAddr = devAddr << 1;
 	I2CStart();						//Start sequence
 	BOOL devAck = I2CWrite(devAddr & 0xFE);		//Initiate write sequence
 	// Check if device is available
 	if(devAck == FALSE)
 	{
 		I2CStop(); 			// Executes a STOP Transition
 		_i2cTimeout = TRUE;	// Set Timeout Flag
 		return 0; 			// and return
 	}
	if(_i2cAddrSize == 2)			// Case of 2 BYTEs register address
	{
		v = (regAddr >> 8) & 0xFF;	// get Most significant byte of address
		I2CWrite(0xFF & v);
	}
	I2CWrite(0xFF & regAddr);		//Send register (r) to read
	I2CRestart();					//Restart
	I2CWrite(devAddr | 0x01); 		//Initiate read sequence to read a byte
	Delay10us(rwDelay);				//Wait
  	v = I2CRead(1);					//Receive lsb
  	I2CStop();						//Stop sequence
  return v;
}

 /**
 * Reads the value of many registers from the I2C device with the specified address. It's possible to specify the start register and the number of registers to read.
 * \param devAddr The address of the device to read. The address must be in 8 bit format.
 * \param regAddr The address of the start register. 
 * \param dest The destination array where put the read values.
 * \param regToRead Number of registers to read.
 * \param rwDelay The delay (expressed in 10us) between the write and the read instruction, since some I2C device requires it.
 * \return the report for the operation.
 */
BOOL I2CReadMulti(BYTE devAddr, unsigned int regAddr, BYTE dest[], unsigned int regToRead, unsigned int rwDelay)
{
	unsigned char rep;
	unsigned int count;
	BOOL report;

	devAddr = devAddr << 1;
 	I2CStart();						//Start sequence
	
	BOOL devAck = I2CWrite(devAddr & 0xFE);		//Initiate write sequence
 	// Check if device is available
 	if(devAck == FALSE)
 	{
 		I2CStop(); 			// Executes a STOP Transition
 		_i2cTimeout = TRUE;	// Set Timeout Flag
 		return FALSE; 		// and return
 	}
	if(_i2cAddrSize == 2)			// Case of 2 BYTEs register address
	{
		rep = (regAddr >> 8) & 0xFF;	// get Most significant byte of address
		I2CWrite(0xFF & rep);
	}
	I2CWrite( 0xFF & regAddr);				//Send register to start reading
	report = I2CRestart();			//Restart
	

	//	Writing sequence terminated, check on restart error
	if (!report)
		return FALSE;
	I2CWrite(devAddr | 0x01); 		//Initiate read sequence to read the registers
	Delay10us(rwDelay);		
	

	//	Reading regToRead bytes from the device
	for (count=0; count<(regToRead-1); count++)
	{
		dest[count] = I2CRead(0);	
		if(_i2cTimeout)
			return FALSE;
	}
  	dest[count] = I2CRead(1);					//Receive lsb
  	if(_i2cTimeout)
		return FALSE;
	report = I2CStop();						//Stop sequence
  return report;
}


 /**
 * Writes one register on the specified device.
 * \param devAddr The address of the device to write. The address is expressed in 7 bit format.
 * \param regAddr The address of the register to write. 
 * \param val Value to write on register.
 * \return None.
 */
void I2CWriteReg(BYTE devAddr, unsigned int  regAddr, BYTE val)
{
	unsigned char reg;
	devAddr = devAddr << 1;
 	I2CStart(); 					//Start sequence
 	BOOL devAck = I2CWrite(devAddr & 0xFE);		//Initiate write sequence
 	// Check if device is available
 	if(devAck == FALSE)
 	{
 		I2CStop(); 			// Executes a STOP Transition
 		_i2cTimeout = TRUE;	// Set Timeout Flag
 		return; 			// and return
 	}
	if(_i2cAddrSize == 2)			// Case of 2 BYTEs register address
	{
		reg = (regAddr >> 8) & 0xFF;	// get Most significant byte of address
		I2CWrite(0xFF & reg);
	}
	I2CWrite(0xFF & regAddr);				//Send register (regAddr) to write
	I2CWrite(val);					//Write (val);
	I2CStop();						//Stop sequence
}


 /**
 * Performs a "bulk write" operation of the number of registers specified, starting from a specific register.
 * \param devAddr The address of the device to read. The address must be in 8 bit format.
 * \param regAddr The address of the start register. 
 * \param src The source array containing the data to write.
 * \param regToWrite Number of registers to write.
 * \return None.
 */
void I2CWriteMulti(BYTE devAddr, unsigned int  regAddr, BYTE* src, unsigned int regToWrite)
{
	unsigned char rep;
	unsigned int count;

	devAddr = devAddr << 1;
 	I2CStart(); 					//Start sequence
	BOOL devAck = I2CWrite(devAddr & 0xFE);		//Initiate write sequence
 	// Check if device is available
 	if(devAck == FALSE)
 	{
 		I2CStop(); 			// Executes a STOP Transition
 		_i2cTimeout = TRUE;	// Set Timeout Flag
 		return; 			// and return
 	}
	if(_i2cAddrSize == 2)			// Case of 2 BYTEs register address
	{
		rep = (regAddr >> 8) & 0xFF;	// get Most significant byte of address
		I2CWrite(0xFF & rep);
	}
	I2CWrite(0xFF & regAddr);				//Send register (regAddr) to write
	for (count=0; count<regToWrite; count++)
	{
		I2CWrite(src[count]);
	}
	I2CStop();						//Stop sequence
}

/*! @} */

/*! @} */
/*! @} */
