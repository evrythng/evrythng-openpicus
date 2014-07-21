/* **************************************************************************																					
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
 
 
/****************************************************************************
  SECTION 	Include
****************************************************************************/

#include "TCPIP Stack/TCPIP.h"
#if defined(STACK_USE_HTTP2_SERVER)
#define __HTTPAPP_C

 
/****************************************************************************
  FUNCTION	HTTP_IO_RESULT HTTPExecuteGet(void)
	
  This function processes every GET request from the pages. For futher 
  information, please see the "standard webserver" example provided.
*****************************************************************************/
HTTP_IO_RESULT HTTPExecuteGet(void)
{
	
	
	return HTTP_IO_DONE;
}


/****************************************************************************
  Section:
	POST Form Handlers
  ***************************************************************************/
#if defined(HTTP_USE_POST)

/*****************************************************************************
  Function:
	HTTP_IO_RESULT HTTPExecutePost(void)

  	This function processes every POST request from the pages. 
  ***************************************************************************/
HTTP_IO_RESULT HTTPExecutePost(void)
{

	return HTTP_IO_DONE;
}

#endif 



/***************************************************************************
  SECTION	Dynamic Variable Callback Functions
  
  In this section are managed the "dynamic variables" of the webserver.
  Dynamic variables are contained in the status.xml file requested by the 
  webpage. 
  For each dynamic variable a callback function named HTTPPrint_varname 
  must be created.
****************************************************************************/

/****************************************************************************
  SECTION 	Authorization Handlers
****************************************************************************/
 

/*****************************************************************************
  FUNCTION	BYTE HTTPNeedsAuth(BYTE* cFile)

  This function is used by the stack to decide if a page is access protected.
  If the function returns 0x00, the page is protected, if returns 0x80, no 
  authentication is required
*****************************************************************************/
#if defined(HTTP_USE_AUTHENTICATION)
BYTE HTTPNeedsAuth(BYTE* cFile)
{
	//	If you want to restrict the access to some page, include it in the folder "protect"
	//	here you can change the folder, or add others
	if(memcmp(cFile, (void*)"protect", 7) == 0)
		return 0x00;		// Authentication will be needed later

	return 0x80;			// No authentication required
}
#endif

/*****************************************************************************
  FUNCTION	BYTE HTTPCheckAuth(BYTE* cUser, BYTE* cPass)
	
  This function checks if username and password inserted are acceptable

  ***************************************************************************/
#if defined(HTTP_USE_AUTHENTICATION)
BYTE HTTPCheckAuth(BYTE* cUser, BYTE* cPass)
{
	if(strcmp((char *)cUser,(char *)"admin") == 0
		&& strcmp((char *)cPass, (char *)"flyport") == 0)
		return 0x80;		// We accept this combination

	return 0x00;			// Provided user/pass is invalid
}
#endif

#endif
