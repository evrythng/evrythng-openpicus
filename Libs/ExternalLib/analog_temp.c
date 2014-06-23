/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        analog_temp.c
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

#include <math.h>
#include "taskFlyport.h"
#include "grovelib.h"
#include "analog_temp.h"

struct Interface *	attachSensorToAnBus (void *,int);

struct An_Temp
{
	const void *class;
	struct Interface *inter;
};

static void *An_Temp_ctor (void * _self, va_list *app)
{
	struct An_Temp *self = _self;
	self->inter = NULL;
	return self;
}	

static void An_Temp_dtor (void * _sensor)
{
	struct An_Temp *sensor = _sensor;
	if(sensor->inter)
	{
		free(sensor->inter->port);
		free(sensor->inter);
	}
}	

static void *An_Temp_attach (void * _board,void *_sensor,int portnumb)
{
	struct An_Temp *sensor = _sensor;
	sensor->inter = attachSensorToAnBus(_board,portnumb);	
	return sensor->inter;
}	


static float An_Temp_get (void * _self,va_list *app)
{
	struct An_Temp *self = _self;
	unsigned int ret = ADCVal(self->inter->port->Pin1);
	float resistance=(float)(1023-ret)*10000/ret; 
	float temperature=1/(log(resistance/10000)/3975+1/298.15)-273.15;
	return temperature;
}	
	
static const struct SensorClass _An_Temp =
{	
	sizeof(struct An_Temp),
	An_Temp_ctor,
	An_Temp_dtor,
	An_Temp_attach,
	0,
	0,
	An_Temp_get,
};

const void *An_Temp = &_An_Temp;


