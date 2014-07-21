/** \file HTTPlib.c
 *  \brief HTTP library
 */

/* **************************************************************************
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 *
 *            openSource wireless Platform for sensors and Internet of Things
 * **************************************************************************
 *  FileName:        HTTPlib.c
 *  Module:          FlyPort
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Stefano Saccucci     1.0     02/21/2014        First release  (core team)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Software License Agreement
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  This is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by
 *  the Free Software Foundation AND MODIFIED BY OpenPicus team.
 *	
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

/**
\defgroup HTTP
@{

The HTTP library contains all the commands to send an HTTP request (POST and GET) and to read the response.
*/

#include "taskFlyport.h"
#include "HTTPlib.h"

static char hex[] = {'\x24','\x26','\x2B','\x2C','\x2F','\x3A','\x3B','\x3D','\x3F','\x40','\x20','\x22','\x3C','\x3E','\x23','\x25','\x7B','\x7D','\x7C','\x5C','\x5E','\x7E','\x5B','\x5D','\x60'};

/// @cond
int HTTP_Read(TCP_SOCKET socket, char * header, int headersize, char * body, int bodysize, int timeout)
{
	int rn=0;
	BOOL fB=0;
	int cnt=0;
	int k=0;
	char bff[2];
	int i=0;
	int nCODE=0;
	char code[4];
	
	header[0]='\0';
	int cntheader = 0;
	
	while(TCPRxLen(socket)<1&&cnt<timeout/10)
	{
		vTaskDelay(10);
		cnt++;
	}
	if(cnt==timeout/10)
		return 0;
	else
	{
		while(TCPRxLen(socket)>0&&k==0)
		{
			if(fB==0)
			{
				TCPRead(socket,bff,1);
				nCODE++;
				
				if(cntheader<headersize-1)
				{					
					strcat(header,bff);
					cntheader++;
				}
				if(bff[0]=='\r'||bff[0]=='\n')
					rn++;
				else
				{				
					if(nCODE>9&&nCODE<13)
						code[i++]=bff[0];
					rn=0;
				}
				if(rn==4&&fB==0)
					fB=1;
			}
			else
			{
				if((k=TCPRxLen(socket))>=bodysize)
					TCPRead(socket,body,bodysize-1);
				else
					TCPRead(socket,body,k);
			}	
		}
		return atoi(code);
	}			
}
/// @endcond

/**
 * Function to send a GET request and to receive the response
 * \param socket - the handle of the socket to use
 * \param host - string with the host (server)
 * \param path_data - string with the path (of file) and data, e.g. "/index.php?param1=val1"
 * \param custom_header - string with custom headers (must include "\r\n", null header = "")
 * \param header - pointer in which to store the header response
 * \param headersize - length of the header array (use ARRAY_SIZE(header))
 * \param body - pointer in which to store the body response
 * \param bodysize - length of the body array (use ARRAY_SIZE(body))
 * \param timeout - timeout period in 10ms
 * \return the HTTP code or 0 for timeout
 */
int HTTP_Get(TCP_SOCKET socket, char * host, char * path_data, char * custom_header, char * header, int headersize, char * body, int bodysize, int timeout) //multipli di 10ms come vTaskDelay
{
	char request[strlen(host)+strlen(path_data)+strlen(custom_header)+50];

	TCPRxFlush(socket);
	
	sprintf(request,"GET %s HTTP/1.1\r\nHOST: %s\r\n%s\r\n\r\n", path_data,host,custom_header);
	UARTWrite(1,request);
	
	TCPWrite(socket,request,strlen(request));
	return HTTP_Read(socket, header, headersize, body, bodysize, timeout);
}

/**
 * Function to send a GET request in easy way (minimal header and fixed timeout(2s)) and to receive the response
 * \param socket - the handle of the socket to use
 * \param host - string with the host (server)
 * \param path_data - string with the path (of file) and data, e.g. "/index.php?param1=val1"
 * \param header - pointer in which to store the header response
 * \param headersize - length of the header array (use ARRAY_SIZE(header))
 * \param body - pointer in which to store the body response
 * \param bodysize - length of the body array (use ARRAY_SIZE(body))
 * \return the HTTP code or 0 for timeout
 */
int HTTP_GetSimple(TCP_SOCKET socket, char * host, char * path_data, char * header, int headersize, char * body, int bodysize)
{
	return HTTP_Get(socket, host, path_data, "", header, headersize, body, bodysize, 200);
}

/**
 * Function to send a POST request and to receive the response
 * \param socket - the handle of the socket to use
 * \param host - string with the host (server)
 * \param path - string with the path (of file), e.g. "/index.php"
 * \param custom_header - string with custom headers (must include "\r\n", null header = "")
 * \param CType - string with the Content-Type header, e.g. "application/x-www-form-urlencoded"
 * \param data - string with the data, e.g. "param1=val1&param2=val2"
 * \param header - pointer in which to store the header response
 * \param headersize - length of the header array (use ARRAY_SIZE(header))
 * \param body - pointer in which to store the body response
 * \param bodysize - length of the body array (use ARRAY_SIZE(body))
 * \param timeout - timeout period in 10ms
 * \return the HTTP code or 0 for timeout
 */
int HTTP_Post(TCP_SOCKET socket, char * host, char * path, char * custom_header, char * CType, char * data, char * header, int headersize, char * body, int bodysize, int timeout) //multipli di 10ms come vTaskDelay
{
	char request[strlen(host)+strlen(path)+strlen(custom_header)+strlen(CType)+strlen(data)+100];

	TCPRxFlush(socket);
	
	sprintf(request,"POST %s HTTP/1.1\r\nHOST: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n%s\r\n%s\r\n", path, host, CType, strlen(data), custom_header,data);
	UARTWrite(1,request);
	
	TCPWrite(socket,request,strlen(request));
	return HTTP_Read(socket, header, headersize, body, bodysize, timeout);
}

/**
 * Function to send a POST request in easy way (minimal header, Content-Type: application/x-www-form-urlencoded and fixed timeout(2s)) and to receive the response
 * \param socket - the handle of the socket to use
 * \param host - string with the host (server)
 * \param path - string with the path (of file), e.g. "/index.php"
 * \param data - string with the data, e.g. "param1=val1&param2=val2"
 * \param header - pointer in which to store the header response
 * \param headersize - length of the header array (use ARRAY_SIZE(header))
 * \param body - pointer in which to store the body response
 * \param bodysize - length of the body array (use ARRAY_SIZE(body))
 * \return the HTTP code or 0 for timeout
 */
int HTTP_PostSimple(TCP_SOCKET socket, char * host, char * path, char * data, char * header, int headersize, char * body, int bodysize) //multipli di 10ms come vTaskDelay
{
	return HTTP_Post(socket, host, path, "", "application/x-www-form-urlencoded", data, header, headersize, body, bodysize, 200);
}

/**
 * Function to send a PUT request and to receive the response
 * \param socket - the handle of the socket to use
 * \param host - string with the host (server)
 * \param path - string with the path (of file), e.g. "/index.php"
 * \param custom_header - string with custom headers (must include "\r\n", null header = "")
 * \param data - string with the data, e.g. "param1=val1&param2=val2"
 * \param header - pointer in which to store the header response
 * \param headersize - length of the header array (use ARRAY_SIZE(header))
 * \param body - pointer in which to store the body response
 * \param bodysize - length of the body array (use ARRAY_SIZE(body))
 * \param timeout - timeout period in 10ms
 * \return the HTTP code or 0 for timeout
 */
int HTTP_Put(TCP_SOCKET socket, char * host, char * path, char * custom_header, char * data, char * header, int headersize, char * body, int bodysize, int timeout) //multipli di 10ms come vTaskDelay
{
	char request[strlen(host)+strlen(path)+strlen(custom_header)+strlen(data)+100];

	TCPRxFlush(socket);
	
	sprintf(request,"PUT %s HTTP/1.1\r\nHOST: %s\r\nContent-Length: %d\r\n%s\r\n%s\r\n", path, host, strlen(data), custom_header,data);
	UARTWrite(1,request);
	
	TCPWrite(socket,request,strlen(request));
	return HTTP_Read(socket, header, headersize, body, bodysize, timeout);
}

/**
 * Function to encode a string in a URL string
 * \param dest - pointer in which to store the URL string
 * \param src - string with the text to convert
 */
void HTTP_URLEncode(char * dest, char * src)
{
	int lenstr=strlen(src);
	int i=0;
	int k=0;
	int j=0;
	int limit=sizeof hex;
	for(i=0;i<lenstr;i++)
	{
		for(k=0;k<limit;k++)
		{
			if(src[i]==hex[k])
			{
				dest[j++]=0x25;
				dest[j++]=(hex[k]>>4)|0x30;
				if((hex[k]&0x0F)<0x0A)
					dest[j++]=(hex[k]&0x0F)|0x30;
				else
					dest[j++]=(hex[k]&0x0F)+55;
				break;
			}
		}
		if(k==limit)
			dest[j++]=src[i];
	}
	dest[j++]='\0';
}

/**
 * Function to calculate the length of the future URL string
 * \param str - string with the text to calculate
 * \return the length of the future URL string
 */
int HTTP_URLEncodeLen(char * str)
{
	int lenstr=strlen(str);
	int lenURL=lenstr;
	int i=0;
	int k=0;
	int limit=sizeof hex;
	for(i=0;i<lenstr;i++)
	{
		for(k=0;k<limit;k++)
		{
			if(str[i]==hex[k])
			{
				lenURL=lenURL+2;
				break;
			}
		}
	}
	return lenURL+1;
}

/**
 * Function to decode a string in a no-URL string
 * \param dest - pointer in which to store the no-URL string
 * \param src - string with the text to convert
 */
void HTTP_URLDecode(char * dest, char * src)
{
	int lenstr=strlen(src);
	int i=0;
	int j=0;
	for(i=0;i<lenstr;i++)
	{
		if(src[i]==0x25)
		{
			dest[j]=(src[i+1]&0x0F)<<4;
			
			if(src[i+2]>64)
				dest[j]=dest[j]|(src[i+2]-55);
			else
				dest[j]=dest[j]|(src[i+2]&0x0F);

			i=i+2;
			j++;
		}
		else
			dest[j++]=src[i];
	}
	dest[j++]='\0';
}

/**
 * Function to calculate the length of the future no-URL string
 * \param str - string with the text to calculate
 * \return the length of the future no-URL string
 */
int HTTP_URLDecodeLen(char * str)
{
	int lenstr=strlen(str);
	int lenURL=lenstr;
	int i=0;
	for(i=0;i<lenstr;i++)
	{
		if (str[i]==0x25)
		{
			lenURL=lenURL-2;
			i=i+2;
		}
	}
	return lenURL+1;
}
