/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        digital_io.c
 *  Dependencies:    OpenPicus libraries
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

#include "taskFlyport.h"
#include "grovelib.h"

struct Interface *attachSensorToDigioBus(void *,int,int);

struct Dig_io
{
	const void *class;
	struct Interface *inter;
	BYTE type;
	BYTE pwm_num;
	float* pwm_frequency;
	float* pwm_duty;
	
};

static void *Dig_io_ctor (void * _self, va_list *app)
{
	struct Dig_io *self = _self;
	self->type = va_arg(*app, const BYTE);
	if(self->type == 4)
	{	
		// PWM sensor type
		self->pwm_num = va_arg(*app, const BYTE);
		self->pwm_frequency = (float *)malloc(sizeof(float));
		self->pwm_duty = (float *)malloc(sizeof(float));
		if(!self->pwm_frequency || !self->pwm_duty)
		{
			free(self);
			return NULL;
		}
		//default values
		*self->pwm_duty = 50;
		*self->pwm_frequency = 1000;
	}
	else // GPIO sensor type
		self->pwm_num = 0;
	self->inter = NULL;
	return self;
}	

static void Dig_io_dtor (void * _sensor)
{
	struct Dig_io *sensor = _sensor;
	if(sensor->type == 4)// PWM type sensor
	{
		free(sensor->pwm_frequency);
		free(sensor->pwm_duty);
	}
	if(sensor->inter)
	{
		free(sensor->inter->port);
		free(sensor->inter);
	}

}	

static void* Dig_io_attach (void * _board,void *_sensor,int n)
{
	struct Dig_io *sensor = _sensor;
	if(sensor->type == 4)
		// pwm type sensor
		PWMInit(sensor->pwm_num,(float) 1000, (float) 50);
	sensor->inter = attachSensorToDigioBus(_board,n,sensor->type);	
	return sensor->inter;

}	

static int Dig_io_set_dig_io (void * _self,va_list *app)
{
	struct Dig_io *self = _self;
	BYTE state = va_arg(*app, const BYTE);
	if(self->type  != 4)		
		IOPut(self->inter->port->Pin1,state); 
	else
	{
		if(state <2)
		{	
			if(state == ON)
				PWMOn(self->inter->port->Pin1, self->pwm_num);
			else
				PWMOff(self->pwm_num);
		}
		else
		{
			float data = va_arg(*app, const float);
			if(state == 2)
			{
				PWMInit(self->pwm_num,data,*self->pwm_duty);
				*self->pwm_frequency = data;
			}
			else
				PWMDuty(data,self->pwm_num);	
		}
	}
	return 0;
}	

static float Dig_io_get_dig_io (void * _self,va_list *app)
{
	struct Dig_io *self = _self;
	int ret = IOGet(self->inter->port->Pin1);
	return (float) ret;
}	

static const struct SensorClass _Dig_io =
{	
	sizeof(struct Dig_io),
	Dig_io_ctor,
	Dig_io_dtor,
	Dig_io_attach,
	0,
	Dig_io_set_dig_io,
	Dig_io_get_dig_io,
};

const void *Dig_io = &_Dig_io;





