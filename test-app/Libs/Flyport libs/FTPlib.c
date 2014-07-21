/** \file FTPlib.c
 *  \brief FTP wrapper for FreeRTOS
 */

/**
\addtogroup net
@{
*/

/**
\defgroup FTPlib FTPlib stack
@{

FTP provides functions to manage FTP client connection. The FTP functions may be divided in "low level" and "high level" functions. The first
ones provide basic functionalities, like FTP client opening/closing and reading/writing strings (data or commands) on FTP sockets. With this kind of 
commands, the user has to manage by himself the timings and the answers of the server and decode them properly. 
The second set of instruction, on the contrary, allows to perform complete FTP operations, like connecting to the server using specified usaername and password,
enter in passive mode, checking the size or existance of a specified file, or reading/writing a data stream directly from/to a file. 
Follows a list of the possible error codes provided by the functions:
 <B>FTP functions error codes: </B>
 <UL>
	<LI><B>FTP_ERR_NOT_CREATED - </B> a problem occured while creating a specificied FTP socket. Not error in connection, maybe some internal problem in Flyport (maybe no more socket available).</LI><BR>
	<LI><B>FTP_ERR_SERV_NOT_FOUND - </B> the FTP server was not found

\section FTP library
*/

/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        FTPlib.c
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Gabriele Allegria    1.0     1/20/2011		   First release  (core team)
 *        ---
 *  Gabriele Allegria
 *  Simone Marra	     1.1	 3/16/2012                Added FTPClientPasv	
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

#include "FTPlib.h"

//	Static functions prototypes
static void errHandling(TCP_SOCKET *FTPErr);
static int FTPMultiWrite(TCP_SOCKET multSock, BYTE mulstr[], int mulLen);
static int FTPwFile(TCP_SOCKET cmdSock, char fileName[], char appStr[], char wMode[]);

//	Static variables
static TCP_SOCKET dataSocket = INVALID_SOCKET;
static int streamStat = 0;
static int ToSend = 0; 
static long streamLen = 0;
static long streamRBytes = 0;

/*****************************************************************************
	LOW LEVEL FUNCTIONS	
*****************************************************************************/

/**
 * Creates an FTP client on specified IP address and port
 * \param ftpaddr - IP address of the remote server. Example: "192.168.1.100" (the char array must be NULL terminated).
 * \param ftpport - Port of the remote server to connect. Example: "1234" (the char array must be NULL terminated).
 * \return - INVALID_SOCKET: the operation was failed. Maybe there are not available sockets.
 * \return - A TCP_SOCKET handle to the created socket. It must be used to access the socket in the program (read/write operations and close socket).
 */
TCP_SOCKET FTPClientOpen (char ftpaddr[] , char ftpport[])
{
	return TCPGenericOpen(ftpaddr , 1 , ftpport , 3);
}



/**
 * Reads the specified number of characters from a FTP socket and puts them into the specified char array
 * \param ftpsockread - The handle of the socket to read (the handle returned by the command FTPClientOpen).
 * \param ftpreadch - The char array to fill with the read characters.
 * \param ftprlen - The number of characters to read. 
 * \warning The length of the array must be AT LEAST = ftprlen+1, because at the end of the operation the array it's automatically NULL terminated (is added the '\0' character).
 * \return None.
 */
void FTPRead(TCP_SOCKET ftpsockread , char ftpreadch[] , int ftprlen)
{
	TCPRead(ftpsockread , ftpreadch , ftprlen);
}



/**
 * Writes an array of characters on the specified socket.
 * \param ftpsockwr - The socket to which data is to be written (it's the handle returned by the command TCPClientOpen or TCPServerOpen).
 * \param ftpstrtowr - Pointer to the array of characters to be written.
 * \param ftpwlen - The number of characters to write.
 * \return The number of bytes written to the socket. If less than ftpwlen, the buffer became full or the socket is not conected.
 */
WORD FTPWrite(TCP_SOCKET ftpsockwr , BYTE* ftpstrtowr , int ftpwlen)
{
	return TCPWrite(ftpsockwr , (char *)ftpstrtowr , ftpwlen);
}



/**
 * Closes the client socket specified by the handle.
 * \param Sockclose - The handle of the socket to close (the handle returned by the command FTPClientOpen).
 * \return None.
 */
void FTPClose (TCP_SOCKET Sockclose)
{
	TCPGenericClose(Sockclose);
	Sockclose = INVALID_SOCKET;
}



/**
 * Verifies the connection of a remote FTP device with the socket.
 * \param sockcon - The handle of the socket to control (the handle returned by the command FTPOpen).
 * \return TRUE - The remote connection is established.
 * \return FALSE - The remote connection is not established.
 */
BOOL FTPisConn(TCP_SOCKET sockcon)
{
	return TCPisConn(sockcon);
}



/**
 * Verifies how many bytes can be read from the specified FTP socket.
 * \param ftpsocklen - The handle of the socket to control (the handle returned by the command FTPOpen).
 * \return The number of bytes available to be read.
 */
WORD FTPRxLen(TCP_SOCKET ftpsocklen)
{
	return TCPRxLen(ftpsocklen);
}




/*****************************************************************************
	LOW LEVEL MACRO FUNCTIONS	
*****************************************************************************/

/**
 * Open a PassiveMode data exchange between Flyport (Client) and the ServerName
 * \param sockpasv - The handle of the socket for commands exchange.
 * \param ServerName - The server data IP returned by the FTP Server
 * \return The TCP_SOCKET to use for data transfer in Passive Mode.
 */
TCP_SOCKET FTPClientPasv(TCP_SOCKET sockpasv, char ServerName[])
{	
	unsigned long int dataport=0;
	char *word;
	char word1[10];
	char buffertot[100];
	char servReply[30];
	int counter = 0;
		
	FTPRxFlush(sockpasv, 100);
	BYTE strpasv[]={"pasv\r\n"};
	FTPMultiWrite(sockpasv,strpasv,6);
	
	while (FTPRxLen(sockpasv) == 0)
	{
		vTaskDelay(10);
		counter++;
		if (counter == 50)
			break;
	}
	counter = 0;
	
	int indsum=0;
	while (FTPRxLen(sockpasv)>0)
	{
		int toread = FTPRxLen(sockpasv);
		FTPRead(sockpasv , (char *) (buffertot+indsum) , toread);
		indsum += toread;
		while (FTPRxLen(sockpasv) == 0)
		{
			vTaskDelay(10);
			counter++;
			if (counter == 20)
				break;
		}		
	}
	
	word = strtok(buffertot,",()");

	int indsub=1;
	while (word != NULL)
	{
		word = strtok(NULL, ",()");
		if (indsub == 1)
		{
			sprintf(servReply, "%s", word);
			strcat(servReply, ".");
		}
		if (indsub == 2)
		{
			strcat(servReply, word);
			strcat(servReply, ".");
		}
		else if (indsub == 3)
		{
			strcat(servReply, word);
			strcat(servReply, ".");
		}
		else if (indsub == 4)
		{
			strcat(servReply, word);
		}
		else if (indsub == 5)
		{
			dataport=atoi(word);
			dataport=dataport*256;
			int xy;
			xy=sprintf(word1,"%li",dataport);			
		}
		else if (indsub == 6)
		{
			dataport += atoi(word);
			sprintf(word1,"%li",dataport);
		}
		indsub++;
	}

	xFrontEndStatRet = 2;	
	ToSend=20;
	sprintf(ServerName, "%s", servReply);
	return TCPGenericOpen(ServerName, TCP_OPEN_RAM_HOST , word1 , 4);	
}



int FTPSendCmd(TCP_SOCKET cmdSock, char *cmdStr, char* res, int resLen)
{ 
	int servReply;
	char code[5];
	FTPRxFlush(cmdSock, 100);
	int wBytes = 0;
	BOOL chkWrite = TRUE;
	
	//	Sending FTP command
	wBytes = FTPMultiWrite(cmdSock, (BYTE*)cmdStr, strlen(cmdStr));
	if (wBytes < strlen(cmdStr))
		chkWrite = FALSE;
	wBytes = FTPMultiWrite(cmdSock, (BYTE*)"\r\n", 2);
	if (wBytes < 2)
		chkWrite = FALSE;
		
	if (!chkWrite)
	{
		if (FTPisConn(cmdSock))
			return FTP_ERR_TX;
		else 
			return FTP_ERR_SERV_DISCONNECTED;
	}		
	//	Reading answer from server
	servReply = FTPAnswer(cmdSock, code);
	if (servReply != 0)
	{
		return FTP_ERR_SERV_TIMEOUT;
	}		
	
	if (res != NULL)
	{
		FTPRxRead(cmdSock, 100, res, resLen);
	}
	else
		FTPRxFlush(cmdSock, 100);
	return atoi(code);
}


int FTPConnect(TCP_SOCKET *FTPConn, char *ftp_addr, char ftp_port[], char ftp_usr[], char ftp_pwd[])
{
	int err_count = 0;
	int code_rep = 0, flush_rep = 0;
	_dbgwrite("Server IP:");
	_dbgwrite(ftp_addr);
	_dbgwrite("\r\nServer port:");
	_dbgwrite(ftp_port);
	_dbgwrite("\r\n");
	
	//	#1 - SOCKET OPENING: A new FTP socket is open to connect to the remote server
	if (FTPisConn(*FTPConn))
	{
		return FTP_CONNECTED;
	}
	*FTPConn = INVALID_SOCKET;
	*FTPConn = FTPClientOpen(ftp_addr, ftp_port);	
	vTaskDelay(50);
	
	//	Socket creation check: if an internal error occured, 
	//	Flyport retries up to 3 times to create the socket
	while ( (*FTPConn == INVALID_SOCKET) && (err_count < 3) )
	{
		err_count++;
		vTaskDelay(20);
		errHandling(FTPConn);
		vTaskDelay(10);
		*FTPConn = FTPClientOpen(ftp_addr, ftp_port);
		vTaskDelay(20);
	}
	
	// EXCEPTION HANDLING: socket was not created
	if (*FTPConn == INVALID_SOCKET)
		return FTP_ERR_NOT_CREATED;
		
	// Socket is created, now let's check the connection
	// Flyport will wait up to 10 seconds for connection with server
	err_count = 0;
	while ( (!FTPisConn(*FTPConn)) && (err_count < 20) )
	{
		_dbgwrite("Server searching...");
		vTaskDelay(50);
		err_count++;
	}
	_dbgwrite("\r\n");
	
	//	EXCEPTION HANDLING: server not found
	if (!FTPisConn(*FTPConn))
	{
		errHandling(FTPConn);
		return FTP_ERR_SERV_NOT_FOUND;
	}
	_dbgwrite("FTPConnect: SERVER FOUND\r\n");
	
	//	Reading the response code from the server 
	//	(expected response code: 220)
	char code[5];
	code_rep = FTPAnswer(*FTPConn, code);
	
	//	EXCEPTION HANDLING: timeout and bad server answer
	if (code_rep != 0)
	{
		errHandling(FTPConn);
		return FTP_ERR_SERV_TIMEOUT;
	}
	if (strcmp(code, "220") != 0)
	{
		errHandling(FTPConn);
		return atoi(code);
	}
	else 
		_dbgwrite("FTPConnect: SERVER ANSWERED\r\n");

 	flush_rep = FTPRxFlush(*FTPConn, 300);

	vTaskDelay(50);
	
	
 	//	#2 - SENDING USERNAME TO SERVER
 	FTPMultiWrite(*FTPConn, (BYTE*)"USER ", 5);
 	FTPMultiWrite(*FTPConn, (BYTE*)ftp_usr, strlen(ftp_usr));
 	FTPMultiWrite(*FTPConn, (BYTE*)"\r\n", 2);

	code_rep = FTPAnswer(*FTPConn, code);
	//	EXCEPTION HANDLING: server timeout
	if (code_rep != 0)
	{
		if (FTPisConn(*FTPConn))
		{
			errHandling(FTPConn);
			return FTP_ERR_SERV_TIMEOUT;
		}
		else
		{
			errHandling(FTPConn);
			return FTP_ERR_SERV_DISCONNECTED;
		}	
	}
	
	//	EXCEPTION HANDLING: wrong answer from server
 	if ( strcmp(code,"331") != 0 )
	{
		errHandling(FTPConn);
 		return atoi(code);
	}
 	else
 		_dbgwrite("USER OK\r\n");
 	flush_rep = FTPRxFlush(*FTPConn, 100);
 		
 		
 	// #3 - SENDING PASSWORD TO SERVER
  	FTPMultiWrite(*FTPConn, (BYTE*)"PASS ", 5);
	if (ftp_pwd != FTP_NO_PASS)
		FTPMultiWrite(*FTPConn, (BYTE*)ftp_pwd, strlen(ftp_pwd));
 	FTPMultiWrite(*FTPConn, (BYTE*)"\r\n", 2);
	
	code_rep = FTPAnswer(*FTPConn, code);
	//	EXCEPTION HANDLING: server timeout
	if (code_rep != 0)
	{
		if (FTPisConn(*FTPConn))
		{
			errHandling(FTPConn);
			return FTP_ERR_SERV_TIMEOUT;
		}
		else
		{
			errHandling(FTPConn);
			return FTP_ERR_SERV_DISCONNECTED;
		}	
	}
	
	//	EXCEPTION HANDLING: wrong password and unknow answer from server
 	if ( strcmp(code,"230") != 0 )
 	{
	 	if (strcmp(code,"530") == 0 )
		{
			errHandling(FTPConn);
	 		return FTP_ERR_WRONG_LOGIN;
		}	
	 	else
		{
			errHandling(FTPConn);
	 		return atoi(code);	
		}
	 }
  	else
 		_dbgwrite("PASSWORD OK\r\n");
 	flush_rep = FTPRxFlush(*FTPConn, 100);
 	//	Everything went OK, returning connected status 	
	return FTP_CONNECTED;
}



/*****************************************************************************
	FILE MANAGEMENT FUNCTIONS	
*****************************************************************************/
/**
 * Performs a check on the remote FTP server for the existence of the specified file. Only the FTP command
 * socket must be provided, the FTP data connection is managed internally by the framework.
 * \param cmdSock - The handle of the socket for commands exchange.
 * \param fileToCheck - the string containing the name of the file to check on the server.
 * \return an int containing the value FTP_FILE_EXIST (0x00) if the file is found
 * \return an error code. Detailed list of error code can be found at the beginning of FTP chapter.
 */
int FTPFileCheck(TCP_SOCKET cmdSock, char fileToCheck[])
{
	int servReply;
	char code[5];
	
	//	#0 - Checking FTP socket connection
	if (!FTPisConn(cmdSock))
		return FTP_SOCK_NOT_CONNECTED;
	//	#1 - Sending SIZE command	
	FTPRxFlush(cmdSock, 100);
	FTPMultiWrite(cmdSock, (BYTE*)"SIZE ",5);
	FTPMultiWrite(cmdSock, (BYTE*)fileToCheck, strlen(fileToCheck));
	FTPMultiWrite(cmdSock, (BYTE*)"\r\n", 2);

	//	#2 - Reading answer from server
	servReply = FTPAnswer(cmdSock, code);
	if (servReply != 0)
	{
		if (FTPisConn(cmdSock))
		{
			errHandling(&cmdSock);
			return FTP_ERR_SERV_TIMEOUT;
		}
		else
		{
			errHandling(&cmdSock);
			return FTP_ERR_SERV_DISCONNECTED;
		}		
	}	
	
	//	#3 - Answer decoding
 	if ( strcmp(code,"550") == 0 )
		return FTP_FILE_NOT_FOUND;
	else if (strcmp(code,"213") == 0 )
		return FTP_FILE_EXIST;
	return atoi(code);
}



long FTPFileSize(TCP_SOCKET cmdSock, char fileName[])
{
	char cmd[strlen(fileName) + 10], len[12];
	int res;
	strcpy(cmd, "SIZE ");
	strcat(cmd, fileName);
	
	FTPRxFlush(cmdSock, 100);
	res = FTPSendCmd(cmdSock, cmd, len, 12);
	if (res < 0)
		return res;
	_dbgwrite("FTPFileSize:");
	_dbgwrite(len);
	if (res == 550)
		return FTP_FILE_NOT_FOUND;
	if (res == 213)
		return atol(len);
	return FTP_ERR_WRONG_ANSWER;
}



int FTPStore(TCP_SOCKET cmdSock, char fileName[], char stoStr[])
{
	return FTPwFile(cmdSock, fileName, stoStr, STOR);
}



int FTPAppend(TCP_SOCKET cmdSock, char fileName[], char appStr[])
{
	return FTPwFile(cmdSock, fileName, appStr, APPE);
}



static int FTPwFile(TCP_SOCKET cmdSock, char fileName[], char appStr[], char wMode[])
{
	dataSocket = INVALID_SOCKET;
	char pasvBuff[16 + strlen(fileName)];	//	buffer for IP address
	int cnt = 0, servReply, wBytes = 0;
	char code[5];
	BOOL chkWrite = TRUE;

	//	#0 - Checking FTP socket connection
	if (!FTPisConn(cmdSock))
		return FTP_SOCK_NOT_CONNECTED;
		
	//	#1 - Connection in passive mode
	dataSocket = FTPClientPasv(cmdSock, pasvBuff); 	
	
	//	Waiting for data socket connection
	while (!FTPisConn(dataSocket))
	{
		cnt++;
		vTaskDelay(10);
		if (cnt == 30)
		{
			errHandling(&dataSocket);
			return FTP_DATA_NO_CONNECTED;
		}
	}	
	
	//	#2 - Append file command APPE, check on TX chars and answer handling
	wBytes = FTPMultiWrite(cmdSock, (BYTE*)wMode,5);
	if (wBytes < 5)
		chkWrite = FALSE;
	wBytes = FTPMultiWrite(cmdSock, (BYTE*)fileName, strlen(fileName));
	if (wBytes < strlen(fileName))
		chkWrite = FALSE;
	wBytes = FTPMultiWrite(cmdSock, (BYTE*)"\r\n", 2);
	if (wBytes < 2)
		chkWrite = FALSE;
	if (!chkWrite)
	{
		if (FTPisConn(cmdSock))
			return FTP_ERR_TX;
		else 
			return FTP_ERR_SERV_DISCONNECTED;
	}
	//	Reading answer from server
	servReply = FTPAnswer(cmdSock, code);
	if (servReply != 0)
	{
		if (FTPisConn(cmdSock))
		{
			errHandling(&cmdSock);
			return FTP_ERR_SERV_TIMEOUT;
		}
		else
		{
			errHandling(&cmdSock);
			return FTP_ERR_SERV_DISCONNECTED;
		}	
	}	
	FTPRxFlush(cmdSock, 100);

 	if ( strcmp(code,"150") != 0 )
	{
		if ( strcmp(code,"550") == 0 )
			return FTP_FILE_ERROR;
		else
			return FTP_ERR_WRONG_ANSWER;
	}
	
	//	#3 - Sending data to append and check on TX chars
	wBytes = FTPMultiWrite(dataSocket, (BYTE*)appStr, strlen(appStr));

	if (wBytes < strlen(appStr))
	{
		if (FTPisConn(cmdSock))
		{	
			errHandling(&dataSocket);
			return FTP_ERR_TX;
		}
		else 
		{	
			errHandling(&dataSocket);
			return FTP_ERR_SERV_DISCONNECTED;
		}
	}	
	errHandling(&dataSocket);

	//	Reading answer from server
	servReply = FTPAnswer(cmdSock, code);
	if (servReply != 0)
	{
		if (FTPisConn(dataSocket))
		{
			errHandling(&dataSocket);
			return FTP_ERR_SERV_TIMEOUT;
		}
		else
		{
			errHandling(&dataSocket);
			return FTP_ERR_SERV_DISCONNECTED;
		}
	}	
	errHandling(&dataSocket);
	//	#4 - Returning 0 if succeded, otherwise the server from code
 	if ( strcmp(code,"226") != 0 )
		return FTP_ERR_TX_NOT_OK;	
	return 0;
}




/*****************************************************************************
	FILE STREAM MANAGEMENT FUNCTIONS	
*****************************************************************************/

int FTPStreamOpen(TCP_SOCKET cmdSock, char fileName[], char mode[])
{
	//	Stream opening started only if stream is not connected
	if ( (dataSocket == INVALID_SOCKET) &&	(streamStat == FTP_STREAM_NOT_CONN) )
	{
		char pasvBuff[16 + strlen(fileName)];	
		int cnt = 0, servReply;
		char code[5];
	
		//	Check on command socket connection
		if ( (!FTPisConn(cmdSock))|| (cmdSock == INVALID_SOCKET) )
			return FTP_SOCK_NOT_CONNECTED;
			
		//	Reading file size
		if (strcmp(mode, RETR) == 0)
		{
			_dbgwrite("StreamOpen: RETR\r\n");
			streamLen = FTPFileSize(cmdSock, fileName);
			if (streamLen < 0)
				return streamLen;
		}
		else if(strcmp(mode, STOR) == 0)
		{
			_dbgwrite("StreamOpen: STOR\r\n");
		}
		else if(strcmp(mode, APPE) == 0)
		{
			_dbgwrite("StreamOpen: APPE\r\n");
		}
		//	Connection in passive mode
		dataSocket = FTPClientPasv(cmdSock, pasvBuff); 
		
		//	Waiting for data socket connection
		while (!FTPisConn(dataSocket))
		{
			cnt++;
			vTaskDelay(10);
			if (cnt == 30)
			{
				errHandling(&dataSocket);
				return FTP_DATA_NO_CONNECTED;
			}
		}	
		
		//	Append file command APPE/STOR/RETR...
		FTPMultiWrite(cmdSock, (BYTE*)mode,5);
		FTPMultiWrite(cmdSock, (BYTE*)fileName, strlen(fileName));
		FTPMultiWrite(cmdSock, (BYTE*)"\r\n", 2);
		//	Reading answer from server
		servReply = FTPAnswer(cmdSock, code);
		if (servReply != 0)
		{
			errHandling(&dataSocket);
			return FTP_ERR_SERV_TIMEOUT;
		}	
		
	 	if ( ( strcmp(code,"150") != 0 ) && ( strcmp(code,"125") != 0 ) )
			return atoi(code);
		if ( (strcmp( mode, APPE) == 0) || (strcmp( mode, STOR) == 0) )
			streamStat = FTP_STREAM_WRITING;
		else if(strcmp( mode, RETR) == 0)
		{
			streamRBytes = 0;
			streamStat = FTP_STREAM_READING;
		}
		return FTP_CONNECTED;
	}
	else
		return FTP_STREAM_INVALID_OP;
}



long FTPStreamWrite(char strWrite[], long toWrite)
{
	long len = 0;
	DWORD tick1, tick2;
	if ( (FTPisConn(dataSocket)) && (streamStat == FTP_STREAM_WRITING) )
	{
		tick1 = TickGetDiv64K();
		while (len < toWrite)
		{ 
			len += FTPWrite(dataSocket, (BYTE*)&strWrite[len], toWrite - len);
			//	Checking timeout on writing operation
			tick2 = TickGetDiv64K();
			if ((tick2 - tick1) > 5)
			{
				//	Timeout occured during writing, a check on data connection is required
				if (!FTPisConn(dataSocket))
				{
					_dbgwrite("FTPStreamWrite: timeout by server disconnection\r\n");
					errHandling(&dataSocket);
					streamStat = FTP_STREAM_NOT_CONN;
					return len;
				}
				else
				{
					_dbgwrite("FTPStreamWrite: timeout\r\n");
					return len;
				}
			}
		}
		return len;
	}
	else if (!FTPisConn(dataSocket))
		return FTP_DATA_NO_CONNECTED;
	else 
		return FTP_STREAM_INVALID_OP;
}



long FTPStreamRead(char dest[], int len, BYTE timeout)
{
	long count = 0, count2 = 0;
	int toRead;
	DWORD tick1, tick2;
	if (streamStat == FTP_STREAM_READING)
	{
		tick1 = TickGetDiv64K();
		while (count < len)
		{
			count2 = 0;
			toRead = FTPRxLen(dataSocket);
			while ((toRead < len) && (toRead + streamRBytes < streamLen))
			{
				toRead = FTPRxLen(dataSocket);
				count2++;
				if (count2 == 3)
					break;
			}	
			//	Resizing bytes to read according to buffer size
			if (toRead > (len - count))
				toRead = len - count;
			FTPRead(dataSocket, &dest[count], toRead);
			count += toRead;
			streamRBytes += toRead;
			
			//	No data to read, checking timeout and chars read
			if (toRead == 0)
			{
				//	check on file ending, if file is not finished, checking timeout
				if (streamRBytes == streamLen)
				{
					_dbgwrite("FTPStreamRead: EOF reached\r\n");
					streamStat = FTP_STREAM_EOF;
					return count;
				}
				tick2 = TickGetDiv64K();
				
				if ( (tick2 -tick1) > timeout)
				{	
					//	Timeout occured during reading, a check on data connection is required
					if (!FTPisConn(dataSocket))
					{
						_dbgwrite("FTPStreamRead: timeout by server disconnection\r\n");
						errHandling(&dataSocket);
						streamStat = FTP_STREAM_NOT_CONN;
						return count;
					}
					else
					{
						_dbgwrite("FTPStreamRead: timeout\r\n");
						return count;
					}
				}
			}
		}
		return count;
	} 
	else if (!FTPisConn(dataSocket))
	{
		errHandling(&dataSocket);
		return FTP_DATA_NO_CONNECTED;
	}
	else 
		return FTP_STREAM_INVALID_OP;
	
}



void FTPStreamClose()
{
	if (FTPisConn(dataSocket))
	{
		FTPClose(dataSocket);
	}
	dataSocket = INVALID_SOCKET;
	streamStat = FTP_STREAM_NOT_CONN;
}



BYTE FTPStreamStat()
{
	return streamStat;
}



/*****************************************************************************
	FTP UTILS FUNCTIONS	
*****************************************************************************/

int FTPAnswer(TCP_SOCKET FTPtoread, char* res_code)
{
	int ind = 0;
	int counter = 0;
	
	//	Waiting fore at least three chars answer...
	while (FTPRxLen(FTPtoread) < 3)
	{
		vTaskDelay(10);
		counter++;
		if (counter == 50)
			return 1;		//	Timeout
	}
	
	while ( (FTPRxLen(FTPtoread)) && (ind < 3) )
	{
		FTPRead( FTPtoread, (res_code + ind), 1);
		ind++;
	}
	return 0;
}



int FTPRxFlush(TCP_SOCKET FTPtoflush, int timeout) 
{	
	int counter = 0, counter2 = 0;
	char res[50];
	int len;
	
	//	Waiting for the first characters
	while (FTPRxLen(FTPtoflush) == 0)
	{
		vTaskDelay(1);
		counter++;
		if (counter == timeout)
			return 1;	//	No chars to flush
	}
	
	counter = 0;

	while (FTPRxLen(FTPtoflush))
	{	
		// 	The size of dummy array res is 50 char, so len is resized, in case there are more chars in buffer
		len = FTPRxLen(FTPtoflush);
		if (len > 49)
			len = 49;
		FTPRead( FTPtoflush, res, len);
		res[len] = 0;
		_dbgwrite(res);
		//	Waiting for new characters on FTP buffer, the timeout is inserted as parameter by the user
		while (FTPRxLen(FTPtoflush) == 0 )
		{
			counter++;
			vTaskDelay(1);
			if (counter == timeout)
			{
				//	Timeout is reached, but a check on the last char is 
				//	performed, since it must be '\r' or '\n'
				if ( (res[len-1]=='\r') || (res[len-1]=='\n') )
					break;
				else
				{
					counter2++;
					if (counter2 == 3)
						break;
					counter = 0;
				}
			}
		}
	}
	return 0;	//	FTP buffer flushed
}



int FTPRxRead(TCP_SOCKET FTPtoflush, int timeout, char *buffer, int totLen) 
{	
	int counter = 0, counter2 = 0;
	int len, count = 0;
	
	//	Waiting for the first characters
	while (FTPRxLen(FTPtoflush) == 0)
	{
		vTaskDelay(1);
		counter++;
		if (counter == timeout)
			return 1;	//	No chars to read
	}
	
	counter = 0;

	while (FTPRxLen(FTPtoflush))
	{	
		// 	The size of dummy array res is 50 char, so len is resized, in case there are more chars in buffer
		len = FTPRxLen(FTPtoflush);
		if (len > totLen)
			len = totLen;
		FTPRead( FTPtoflush, &buffer[count], len);
		count += len;
		
		//	Waiting for new characters on FTP buffer, the timeout is inserted as parameter by the user
		while (FTPRxLen(FTPtoflush) == 0 )
		{
			counter++;
			vTaskDelay(1);
			if (counter == timeout)
			{
				//	Timeout is reached, but a check on the last char is 
				//	performed, since it must be '\r' or '\n'
				if ( (buffer[count-1]=='\r') || (buffer[count-1]=='\n') )
					break;
				else
				{
					counter2++;
					if (counter2 == 3)
						break;
					counter = 0;
				}
			}
		}
	}
	return 0;	//	FTP buffer read
}



static void errHandling(TCP_SOCKET *FTPErr)
{
	FTPClose(*FTPErr);
	*FTPErr = INVALID_SOCKET;
}



static int FTPMultiWrite(TCP_SOCKET multSock, BYTE multStr[], int multLen)
{
	int count = 0, limit = 0;
	while ( (count < multLen) && (limit < 1000) )
	{
		count += FTPWrite(multSock, &multStr[count], multLen - count);
		limit++;
	}
	return count;
}
