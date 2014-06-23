/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        lib_groveboard.c
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
 
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "groveboard.h"
#include "grovelib.h"


static void *new_sensor(const void *,va_list ap);
static void *new_board(const void *,va_list ap);

unsigned char flag;

	
void *new(const void *_class, ...)
{
	va_list ap;
	va_start(ap,_class);

	if (*( size_t *) _class == *(size_t *) Board)
	{
		return new_board(_class,&ap);			
	}
	else
	{
		return new_sensor(_class,&ap);			
		
	}
	

}
 
void delete_(void *_self)
{
	struct SensorClass **self = _self;
	if(self && *self && (*self)->dtor)
	{
		(*self)->dtor(self);
		free(self);
	}
}

void delet_(void **_self)
{
	*_self = NULL;
	
}

int attachToBoard(void *board,void *sensor,int numb)
{

	const struct SensorClass **sen = sensor;
	if(sensor && *sen && (*sen)->attach )
	{
		if(!(*sen)->attach(board,sensor,numb))
			return -1;
		return 0;
	}
	else
		return -1;

}

int configure(void *self, ...)
{
	va_list ap;
	va_start(ap,self);
	const struct SensorClass **cp = self;
	if(self && *cp && (*cp)->config)
		return (*cp)->config(self,&ap);
	va_end(ap);
	return -1;
}


int set(void *self,...)
{
	va_list ap;
	va_start(ap,self);
	const struct SensorClass **cp = self;
	if(self && *cp && (*cp)->set_)
		return (*cp)->set_(self,&ap);
	return -1;
}

float get(void *self, ...)
{
	va_list ap;
	va_start(ap,self);
	const struct SensorClass **cp = self;
	flag = 0;
	if(self && *cp && (*cp)->get_)
	{
		return (*cp)->get_(self,&ap);
	}
	else
	{
		flag = 1;
		return -1;
	}
}


unsigned char readError(void)
{
	return flag;
}



static void *new_board(const void *_class,va_list ap)
{
	const struct BoardClass *class = _class;
	void *p = calloc(1, class->size);
	if(!p)
	{
		free(p);
		return NULL;
	}
	*(const struct BoardClass **)p = class;
	if(class->ctor)
	{
		p = class->ctor(p, ap);
		if(!p)
			return NULL;
		va_end(ap);
	}

	return p;

}

static void *new_sensor(const void *_class,va_list ap)
{
	const struct SensorClass *class = _class;
	void *p = calloc(1, class->size);
	if(!p)
	{
		free(p);
		return NULL;
	}
	*(const struct SensorClass **)p = class;
	if(class->ctor)
	{
		p = class->ctor(p, ap);
		if(!p)
			return NULL;
		va_end(ap);

	}
	return p;
}


