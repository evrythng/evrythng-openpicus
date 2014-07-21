/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        HWlib.h
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

#ifndef __SOFTLIB_H
#define __SOFTLIB_H

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HWmap.h"

// FreeRTOS includes
#include "portmacro.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define USE_RTCC_LIB
//	UART general defines
#define UART_BUFFER_SIZE 	256
#define UART_PORTS			4
#define UART_DBG_DEF_BAUD	19200
#define UART_BUFFER_SIZE_1 	UART_BUFFER_SIZE
#define UART_BUFFER_SIZE_2 	256
#define UART_BUFFER_SIZE_3 	256
#define UART_BUFFER_SIZE_4 	256


//	WiFi Status defs
#define NOT_CONNECTED		0
#define CONNECTING			1
#define	CONNECTED			2
#define CONNECTION_LOST		3
#define CONNECTION_FAILED	4
#define STOPPING			5
#define TURNED_OFF			6
#define SCANNING			7
#define GETTING_RSSI		8
#define RECONNECTING		9

#define WF_CUSTOM		0
#define WF_DEFAULT		1

#define ETH_CUSTOM		0
#define ETH_DEFAULT		1

	
void HWInit(int);
/*****************************************************************************
 * Section:																	 *
 * ADC - Declarations for the ADC module.									 *
*****************************************************************************/


#define Vref2V		(3ul)
#define Vref3V		(0ul)

int ADCVal(int ch);
void ADCInit();
void ADCAttach(int ch);
void ADCDetach(int ch);
	
/*****************************************************************************
 * Section:																	 *
 * Digital IOs - Declarations and defines for digital inputs and outputs.	 *
*****************************************************************************/
void IOPut(int io,int putval);
void IOInit(int io, int putval);
int IOGet(int io);
int IOButtonState(int io);
#if (defined (FLYPORT_G) || defined(FLYPORT_LITE))
	void PowerLed(int val);
#endif

#define	ioput	IOPut
#define	ioinit	IOInit
#define	ioget	IOGet
#define	iobuttonstate	IOButtonState

#define	released			(1)
#define RELEASED			released
#define	Released			released
#define pressed				(2)
#define PRESSED				pressed
#define Pressed				pressed

	
	
/*****************************************************************************
  Section:
	Generic states defines.
*****************************************************************************/
		
#define off			(0)
#define OFF			(0)
#define Off			(0)
#define on			(1)
#define ON			(1)
#define On			(1)
#define TOGGLE		(2)
#define toggle		(2)
#define Toggle		(2)

#define out			(0)
#define OUT			(0)
#define Out			(0)
#define in			(1)
#define IN			(1)
#define In			(1)
#define inup		(2)
#define INup		(2)
#define INUp		(2)
#define indown		(3)
#define INdown		(3)
#define INDown		(3)

#define UART1RX		(5)
#define UART1CTS	(6)
#define UART2RX		(7)
#define UART2CTS	(8)
#define UART3RX		(9)
#define UART3CTS	(10)
#define UART4RX		(11)
#define UART4CTS	(12)
#define EXT_INT2	(13)
#define EXT_INT3	(14)
#define EXT_INT4	(15)
#define SPICLKIN	(16)
#define SPI_IN		(17)
#define SPI_SS		(18)
#define TIM_4_CLK	(19)

#define UART1TX		(31)
#define UART1RTS	(32)
#define UART2TX		(33)
#define UART2RTS	(34)
#define UART3TX		(35)
#define UART3RTS	(36)
#define UART4TX		(37)
#define UART4RTS	(38)
#define SPICLKOUT	(39)
#define SPI_OUT		(40)
#define SPI_SS_OUT	(41)

#define RESET_PPS 	(50)

	
/*****************************************************************************
  Section:
	UART module: declarations for UART module.
*****************************************************************************/
#define uartinit		UARTInit
#define uarton			UARTOn
#define uartoff			UARTOff
#define uartflush		UARTFlush
#define uartbuffersize	UARTBufferSize
#define uartwrite		UARTWrite
#define uartread		UARTRead	
#define uartbuffer		UARTBuffer

void UARTInit(int port,long int baud);
void UARTOn(int port);
void UARTOff(int port);
void UARTRxInt(int port);
void UARTFlush(int port);
int UARTBufferSize(int port);
void UARTWrite(int port, char *buffer); 
int UARTRead (int , char* , int);
void UARTWriteCh(int , char);
//static
BOOL DownloadMPFS(void);
void _dbgwrite(char* dbgstr);

/*************************************************************************************
	Section:
	PWM Module
*************************************************************************************/
void PWMInit(BYTE pwm, float freq, float dutyc);
void PWMOn(BYTE io, BYTE pwm);
void PWMOff(BYTE pwm);
void PWMDuty(float duty, BYTE pwm);
	
	
/*************************************************************************************
	Section:
	I2C Module
*************************************************************************************/	
#define HIGH_SPEED	0x0025
#define LOW_SPEED	0x009D
void I2CInit(BYTE I2CSpeed);
BOOL I2CStart();
BOOL I2CRestart();
BOOL I2CStop();
BOOL I2CGetDevAck(BYTE devAddress);
BOOL I2CWrite(BYTE data);
BYTE I2CRead(BYTE ack);
char I2CReadReg(BYTE devAddr, unsigned int regAddr, unsigned int rwDelay);
void I2CWriteReg(BYTE devAddr, unsigned int regAddr, BYTE val);
BOOL I2CReadMulti(BYTE devAddr, unsigned int regAddr, BYTE dest[], unsigned int regToRead, unsigned int rwDelay);
void I2CWriteMulti(BYTE devAddr, unsigned int regAddr, BYTE* src, unsigned int regToWrite);
BOOL I2CTimeout();
void I2CAddrSizeSet(BYTE addrSize);
BYTE I2CAddrSizeGet();
#endif
