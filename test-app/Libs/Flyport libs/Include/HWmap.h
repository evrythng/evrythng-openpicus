/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        HWmap.h
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
  *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Gabriele Allegria    1.0     1/20/2011		   First release  (core team)
 *  Andrea Seraghiti     2.2     11/21/2011        Added ethernet support
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
/*****************************************************************************
 *								--- CLOCK FREQUENCY ---					 	 *
 ****************************************************************************/
#ifndef __MAP_H
#define __MAP_H

#define GetSystemClock()		(32000000ul)      // Hz
#define GetInstructionClock()	(GetSystemClock()/2)
#define GetPeripheralClock()	GetInstructionClock()	

// Three Flyport model supported: FLYPORT_B, FLYPORT_G and FLYPORT_ETH
#define FLYPORT_G

//	Old defines compatibility
#if defined (FLYPORT_ETH)
	#define FLYPORTETH
#endif

#if (defined (FLYPORT_B)||defined(FLYPORT_G))
	#define FLYPORT
#endif

/*****************************************************************************
 *							--- WIFI MODULE MAPPING ---					 	 *
 ****************************************************************************/
#if defined (FLYPORT_G)
	#define MRF24WG
	#define FLYPORT_WF
	
#elif defined (FLYPORT_B)
	#define FLYPORT_WF
#endif

#if defined (FLYPORT_WF)
	
	#define WF_CS_TRIS			(TRISGbits.TRISG9)
	#define WF_CS_IO			(LATGbits.LATG9)
	#define WF_SDI_TRIS			(TRISGbits.TRISG7)
	#define WF_SCK_TRIS			(TRISGbits.TRISG6)
	#define WF_SDO_TRIS			(TRISGbits.TRISG8)
	#define WF_RESET_TRIS		(TRISEbits.TRISE7)
	#define WF_RESET_IO			(LATEbits.LATE7)
	#define WF_INT_TRIS	    	(TRISBbits.TRISB2)  // INT1
	#define WF_INT_IO			(PORTBbits.RB2)
	#define WF_HIBERNATE_TRIS	(TRISEbits.TRISE6)
	#define	WF_HIBERNATE_IO		(LATEbits.LATE6)
	#define WF_INT_EDGE		    (INTCON2bits.INT1EP)
	#define WF_INT_IE			(IEC1bits.INT1IE)
	#define WF_INT_IF			(IFS1bits.INT1IF)
	
	#define WF_SSPBUF			(SPI1BUF)
	#define WF_SPISTAT			(SPI1STAT)
	#define WF_SPISTATbits		(SPI1STATbits)
	#define WF_SPICON1			(SPI1CON1)
	#define WF_SPICON1bits		(SPI1CON1bits)
	#define WF_SPICON2			(SPI1CON2)
	#define WF_SPI_IE			(IEC0bits.SPI1IE)
	//#define WF_SPI_IP			(IPC2bits.SPI1IP)
	#define WF_SPI_IF			(IFS0bits.SPI1IF)

	//	External SPI flash connections
	#define SPIFLASH_CS_TRIS		(TRISDbits.TRISD6)
	#define SPIFLASH_CS_IO			(LATDbits.LATD6)
	#define SPIFLASH_SCK_TRIS		(TRISDbits.TRISD5)
	#define SPIFLASH_SDI_TRIS		(TRISBbits.TRISB4)
	#define SPIFLASH_SDI_IO			(PORTBbits.RB4)
	#define SPIFLASH_SDO_TRIS		(TRISBbits.TRISB5)
	#define SPIFLASH_SPI_IF			(IFS5bits.SPI3IF)
	#define SPIFLASH_SSPBUF			(SPI3BUF)
	#define SPIFLASH_SPICON1		(SPI3CON1)
	#define SPIFLASH_SPICON1bits	(SPI3CON1bits)
	#define SPIFLASH_SPICON2		(SPI3CON2)
	#define SPIFLASH_SPISTAT		(SPI3STAT)
	#define SPIFLASH_SPISTATbits	(SPI3STATbits)

#elif defined (FLYPORT_ETH)

	#define ENC100_INTERFACE_MODE			5
	#define ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING
	#define ENC100_TRANSLATE_TO_PIN_ADDR(a)		((((a)&0x0100)<<6) | ((a)&0x00FF))
	
	#define SPIFLASH_CS_TRIS	(TRISBbits.TRISB4)
	#define SPIFLASH_CS_IO		(LATBbits.LATB4)
	#define SPIFLASH_SCK_TRIS	(TRISGbits.TRISG9)
	#define SPIFLASH_SDI_TRIS	(TRISGbits.TRISG7)
	#define SPIFLASH_SDI_IO		(PORTGbits.RG7)
	#define SPIFLASH_SDO_TRIS	(TRISGbits.TRISG8)
	
	#define SPIFLASH_SPI_IF		(IFS5bits.SPI3IF)
	#define SPIFLASH_SSPBUF		(SPI3BUF)
	#define SPIFLASH_SPICON1	(SPI3CON1)
	#define SPIFLASH_SPICON1bits	(SPI3CON1bits)
	#define SPIFLASH_SPICON2	(SPI3CON2)
	#define SPIFLASH_SPISTAT	(SPI3STAT)
	#define SPIFLASH_SPISTATbits	(SPI3STATbits)

#endif

/*****************************************************************************
 *				--- HARDWARE MAPPING ---					 *
 ****************************************************************************/
	//	Hardware configuration
	#define HWDEFAULT	(1)
	#define	hwdefault	HWDEFAULT 	

	//	IOs
	#define p1					(1)
	#define P1					(1)
	#define p2					(2)
	#define P2					(2)
	#define p3					(3)
	#define P3					(3)
	#define p4					(4)
	#define P4					(4)	
	#define p5					(5)
	#define P5					(5)	
	#define p6					(6)
	#define P6					(6)
	#define p7					(7)
	#define P7					(7)
	#define p8					(8)
	#define P8					(8)
	#define p9					(9)
	#define P9					(9)
	#define p10					(10)
	#define P10					(10)
	#define p11					(11)
	#define P11					(11)
	#define p12					(12)
	#define P12					(12)
	#define p13					(13)
	#define P13					(13)
	#define p14					(14)
	#define P14					(14)
	#define p15					(15)
	#define P15					(15)
	#define p17					(17)
	#define P17					(17)
	#define p18					(18)
	#define P18					(18)
	#define p19					(19)
	#define P19					(19)
	#define p20					(20)
	#define P20					(20)
	#define p21					(21)
	#define P21					(21)
	#define p23					(23)
	#define P23					(23)
	#define p24					(24)
	#define P24					(24)
	#define p25					(25)
	#define P25					(25)

	#define p26					(26)
	#define P26					(26)
	#define p27					(27)
	#define P27					(27)
	#define p28					(28)
	#define P28					(28)
	#define p29					(29)
	#define P29					(29)
	#define p30					(30)
	#define P30					(30)
	#define p31					(31)
	#define P31					(31)
	#define p32					(32)
	#define P32					(32)
	#define p33					(33)
	#define P33					(33)
	#define p34					(34)
	#define P34					(34)
	
	#define I1					(5)
	#define i1					(5)
	#define I2					(7)
	#define i2					(7)
	#define I3					(9)
	#define i3					(9)
	#define I4					(11)
	#define i4					(11)
	#define I5					(2)
	#define i5					(2)
	#define O1					(4)
	#define o1					(4)
	#define O2					(6)
	#define o2					(6)
	#define O3					(17)
	#define o3					(17)
	#define O4					(19)
	#define o4					(19)
	#define O5					(21)
	#define o5					(21)
	
	#define D1In				(5)
	#define d1in				(5)
	#define D2In				(7)
	#define d2in				(7)
	#define D3In				(9)
	#define d3in				(9)
	#define D4In				(11)
	#define d4in				(11)
	#define D5In				(2)
	#define d5in				(2)
	#define D1Out				(4)
	#define d1out				(4)
	#define D2Out				(6)
	#define d2out				(6)
	#define D3Out				(17)
	#define d3out				(17)
	#define D4Out				(19)
	#define d4out				(19)
	#define D5Out				(21)
	#define d5out				(21)
	
	//	ADC
#if defined (FLYPORT_WF)
	#define ADCCHANNELSL		(0x3F3F)
#elif defined (FLYPORT_ETH)
	#define ADCCHANNELSL		(0xBF1F)
#endif
	
	#define ADCEnable	(AD1CON1bits.ADON)
	#define ADCVref		(AD1CON2bits.VCFG)
	
#endif
