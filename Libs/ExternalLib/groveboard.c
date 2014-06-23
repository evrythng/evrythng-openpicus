/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        groveboard.c
 *  Module:          FlyPort WI-FI - FlyPort ETH
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Vicca Davide	     1.0     2/09/2012		   First release  
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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "taskFlyport.h"
#include "grovelib.h"
#include "groveboard.h"

extern int an[];
extern int *AD1CFGL;

BYTE g_d1[] = { p2, p5, p10, p12};
BYTE g_d2[] = { p4, p6, p11, p14};	
BYTE g_dtype[] = { D5VR_TYPE, D5VR_TYPE, D5VR_TYPE, D5VR_TYPE, D5VR_TYPE, D3VR_TYPE};
BYTE g_dshared[] = { NOT_SHARED, NOT_SHARED, NOT_SHARED, NOT_SHARED, NOT_SHARED, NOT_SHARED};
//SPI port type 
BYTE g_SCK[] = { p18 }; 
BYTE g_MISO[] ={ p7}; 
BYTE g_MOSI[] = { p17 };
BYTE g_SPI_CS[] ={ p8 }; 
//I2C port type 
BYTE g_I2CSCL[] = { p1 };
BYTE g_I2CSDA[] ={ p3 }; 
//AN ports type
BYTE g_an[] ={ 1, 2, 3 }; 
BYTE g_atype[] = {AN_TYPE, AN_TYPE, AN_TYPE, AN_TYPE};
BYTE g_ashared[] = { NOT_SHARED, NOT_SHARED, NOT_SHARED, NOT_SHARED};


struct Board
{
	const void *class;
	int boardType;
	struct Interface *i2cstart;
	struct Interface *i2cnext;
	struct Interface *diostart;
	struct Interface *dionext;
	struct Interface *anistart;
	struct Interface *aninext;
	struct Interface *uartstart;
	struct Interface *uartnext;
};

BOOL GroveStarted = FALSE;


static void *Board_ctor (void * _self, va_list *app)
{
	// to provide digital operations on p18 (Analog input 4, or RB6/AN6/PGEC2)
	// it should be removed the analog initializzation:
	AD1CON1bits.ADON = 0;
	*AD1CFGL = (ADCCHANNELSL | 0x40);		//	ADC channels enabled are the old ADCCHANNELSL but with AN6 set as digital...
	an[4] = 0;
	AD1CON1bits.ADON = 1;
	
	//SPI2 pins configuration:
	IOInit(p7,	SPI_IN);
	IOInit(p18, SPICLKOUT);
	IOInit(p17, SPI_OUT);
	// init CS line as ouput, default high
	IOInit(p18, out); // SD-CS line
	IOPut(p18, on);
	// sd card detect line as input, with pullup
	IOInit(p19, inup); // SD-CS line
	
	// pullup and pulldown resistors:
	_CN24PDE = 1;	// SCK pulldown
	_CN26PUE = 1;	// SDO pullup
	_CN72PUE = 1;	// SDI pullup
	
	struct Board *self = _self;
	self->boardType = va_arg(*app, const BYTE);
	self->i2cstart = NULL;
	self->diostart = NULL;
	self->anistart = NULL;
	self->uartstart = NULL;
	return self;
	
}


struct Interface *attachSensorToI2CBus(void *_board,int i2cbus,int add)
{
	struct Board *self = _board;
	struct Interface *p;
	self->i2cnext = self->i2cstart;
	if(!self->i2cstart)
	{
		self->i2cstart = (struct Interface *)malloc(sizeof(struct Interface));
		self->i2cnext = self->i2cstart;
		self->i2cnext->port = NULL;
	}	
	if(!self->i2cstart)
		return NULL;
	p = self->i2cnext;
	if(!p->port)
	{	
		p->port = (GrovePort *) malloc(sizeof (GrovePort));
		if(!p->port)
		{
			free(p);
			return NULL;
		}	
		else
		{
			//internal pull-up for SCL and SDA wires
			CNPU6bits.CN83PUE = 1;
			CNPU6bits.CN84PUE = 1;
			p->port->Pin1 = g_I2CSCL[i2cbus];
			p->port->Pin2 = g_I2CSDA[i2cbus];
			p->port->Use = DOUBLE_PORT;
			p->next = NULL;
			I2CInit(LOW_SPEED);
		}
	}
	return p;

}		

struct Interface *attachSensorToDigioBus(void *_board,int n,int type)
{
	struct Board *self = _board;
	struct Interface *p;
	if(self->diostart == NULL)
	{
		self->diostart = (struct Interface *)malloc(sizeof(struct Interface));
		if(!self->diostart)
			return NULL;
		self->dionext = self->diostart;
		self->dionext->port = NULL;
		p = self->dionext;
	}
	else
	{
		self->dionext = self->diostart;
		p = self->dionext;
		while(p != NULL)
		{
			p = p->next;
		}
		if(p == NULL)
		{
			p = (struct Interface *)malloc(sizeof(struct Interface));
			if(!self->diostart)
				return NULL;
			p->port = NULL;
		}	
	}

	if(p->port == NULL)	
	{	
		p->port = (GrovePort *) malloc(sizeof (GrovePort));
		if(!p->port)
		{
			free(p);
			return NULL;
		}
		p->port->Pin1 = g_d1[n];
		
		if(type != 4 && type != 5)
		{	
			IOInit(p->port->Pin1, type);
			p->port->Use = SINGLE_PORT;
		}
		else if(type == 5)
		{
			IOInit(p->port->Pin1, OUT);
			p->port->Pin2 = g_d2[n];
			IOInit(p->port->Pin2, OUT);
			p->port->Use = DOUBLE_PORT;			
		}
		p->next = NULL;
	}
	return p;
}


struct Interface *	attachSensorToAnBus (void *_board,int n)
{
	struct Board *self = _board;
	struct Interface *p;

	if(self->anistart == NULL)
	{
		self->anistart = (struct Interface *)malloc(sizeof(struct Interface));
		if(!self->anistart)
			return NULL;
		self->aninext = self->anistart;
		self->aninext->port = NULL;
		p = self->aninext;
	}
	else
	{
		self->aninext = self->anistart;
		p = self->aninext;
		while(p != NULL)
		{
			p = p->next;
		}
		if(p == NULL)
		{
			p = (struct Interface *)malloc(sizeof(struct Interface));
			if(!p)
				return NULL;
			p->port = NULL;
		}	
	}

	if(p->port == NULL)
	{	
		p->port = (GrovePort *) malloc(sizeof (GrovePort));
		if(!p->port)
		{
			free(p);
			return NULL;
		}
		p->port->Pin1 = g_an[n];
		p->port->Use = SINGLE_PORT;		
		p->next = NULL;
	}
	return p;
		
}
	
struct Interface *attachSensorToUartBus(void *_board,int port,int speed,BYTE module,BYTE interf)
{
	struct Board *self = _board;
	struct Interface *p;
	if(self->uartstart == NULL)
	{
		self->uartstart = (struct Interface *)malloc(sizeof(struct Interface));//punta al primo bus DIG_IO
		if(!self->uartstart)
			return NULL;
		self->uartnext = self->uartstart;
		self->uartnext->port = NULL;
		p = self->uartnext;
	}
	else
	{
		self->uartnext = self->uartstart;
		p = self->uartnext;
		while(p != NULL)
		{
			p = p->next;
		}
		if(p == NULL)
		{
			p = (struct Interface *)malloc(sizeof(struct Interface));
			if(!p)
				return NULL;
			p->port = NULL;
		}	
	}

	if(p->port == NULL)
	{	
		p->port = (GrovePort *) malloc(sizeof (GrovePort));
		if(!p->port)
		{
			free(p);
			return NULL;
		} 
		p->port->Pin1 = g_d1[port];
		p->port->Pin2 = g_d2[port];
		p->port->Use = DOUBLE_PORT;
		p->next = NULL;
		if(module == 1)
			return p;//Uart1 already initialized
		// else initialize other uart modules
		IOInit(p->port->Pin1,interf);
		IOInit(p->port->Pin2,(interf+26));
		UARTInit(module,speed);
		UARTOn(module);
	}
	return p;	
}
	

static const struct BoardClass _Board =
{	
	sizeof(struct Board),	
	Board_ctor,
	0,
	0,
};

const void *Board = &_Board;







