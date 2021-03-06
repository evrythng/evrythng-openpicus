#include "taskFlyport.h"
#include "evrythng.h"
#include "evrythng_utils.h"

#include <stdio.h>
#include "HTTPlib.h"
#include "cJSON.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>

TCP_SOCKET socket = INVALID_SOCKET;

char respHeader[550];
char respBody[550];

/**
* Internal utils
*/
static void connect()
{
	respHeader[0]='\0';
	respBody[0]='\0';

	socket = TCPClientOpen(EVRYTHNG_API_HOST, EVRYTHNG_API_PORT);	
	while(!TCPisConn(socket))
    {
		vTaskDelay(30);
		_dbgwrite(".");
    }
	_dbgwrite("\r\nTCP connection OK\r\n");
		
	TCPSSLStart(socket);
	char buff[100];
	sprintf(buff, "SSL stat: %d\r\n", TCPSSLStatus(socket));
	_dbgwrite(buff);
	while(TCPSSLStatus(socket) == 1);
	sprintf(buff, "SSL stat: %d\r\n", TCPSSLStatus(socket));		
	_dbgwrite(buff);
	
}

static void disconnect()
{
	TCPClientClose(socket);
}

static void printResponse()
{
	_dbgwrite("\n\rResponse Header:\n\r");
	_dbgwrite(respHeader);
	_dbgwrite("\n\rResponse Body:\n\r");
	_dbgwrite(respBody);
	_dbgwrite("\n\r");
}


static char* createHeader(char*apikey){
	char * header = malloc(strlen(AUTHORIZATION_HEADER_KEY)+strlen(apikey)+1);	
	sprintf(header, "%s%s", AUTHORIZATION_HEADER_KEY, apikey);
	return header;
}

static  char* createHeader_BL(char*apikey){
	char * header = malloc(strlen(AUTHORIZATION_HEADER_KEY)+strlen(apikey)+3);	
	sprintf(header, "%s%s\r\n", AUTHORIZATION_HEADER_KEY, apikey);
	return header;
}

static char* createHeader_ContentType(char*apikey, char*contentType){
	char * header = malloc(strlen(AUTHORIZATION_HEADER_KEY)+strlen(apikey)+strlen(CONTENT_TYPE_HEADER_KEY)+strlen(contentType)+5);
	sprintf(header, "%s%s\r\n%s%s\r\n", AUTHORIZATION_HEADER_KEY, apikey, CONTENT_TYPE_HEADER_KEY,contentType);
	return header;
}

static char* createGetPropertyURLPath(char* thngId, char* propertyName){
	char * url = malloc(strlen(THNGS_PATH)+strlen(thngId)+strlen(PROPERTIES_PATH)+strlen(propertyName)+strlen(FROM_LATEST)+1);
	sprintf(url, "%s%s%s%s%s", THNGS_PATH, thngId, PROPERTIES_PATH, propertyName, FROM_LATEST);	
	return url;
};

static char* createUpdatePropertyURLPath(char* thngId, char* propertyName){
	char * url = malloc(strlen(THNGS_PATH)+strlen(thngId)+strlen(PROPERTIES_PATH)+strlen(propertyName)+1);
	sprintf(url, "%s%s%s%s", THNGS_PATH, thngId, PROPERTIES_PATH, propertyName);
	return url;
};

static char* createPostActionURLPath(char* actionType){
	char * url = malloc(strlen(ACTIONS_PATH)+strlen(actionType)+1);
	sprintf(url, "%s%s", ACTIONS_PATH, actionType);
	return url;
}

static char* createGetLastActionURLPath(char * thngId){
	char * url = malloc(strlen(LAST_ACTION_PATH)+strlen(thngId)+1);
	sprintf(url, "%s%s", LAST_ACTION_PATH, thngId);
	return url;
}

/**
* EVT interface
*/
int evt_GetPropertyValue(char* apikey, char* thngId, char* propertyName, Property *property)
{
	connect();
	
	char * urlPath = createGetPropertyURLPath(thngId, propertyName);
	char * header = createHeader(apikey);
	
	int responseCode = HTTP_Get(socket, EVRYTHNG_API_HOST, urlPath, header, respHeader, ARRAY_SIZE(respHeader), respBody, ARRAY_SIZE(respBody),200);
	
	if (responseCode == 200)
	{
		parseProperty(property, respBody);
	}

	disconnect();
	free(urlPath);
	free(header);
	
	return responseCode;	
};

int evt_UpdatePropertyValue(char* apikey, char* thngId, char* propertyName, Property* property)
{
	connect();

	char * urlPath = createUpdatePropertyURLPath(thngId, propertyName);
	char * header = createHeader_ContentType(apikey,"application/json");
	char * jsonString = serializeProperty(property);
	
	int responseCode = HTTP_Put(socket, EVRYTHNG_API_HOST, urlPath, header, jsonString, respHeader, ARRAY_SIZE(respHeader), respBody, ARRAY_SIZE(respBody),200);
	
	disconnect();	
	free(jsonString);
	free(urlPath);
	free(header);
	
	return responseCode;	
};

int evt_PostAction(char* apikey, Action * action)
{
	connect();

	char * urlPath = createPostActionURLPath(action->type);
	char * header = createHeader_BL(apikey);
	char * jsonString = serializeAction(action);
		
	int responseCode = HTTP_Post(socket, EVRYTHNG_API_HOST, urlPath, header, "application/json", jsonString, respHeader, ARRAY_SIZE(respHeader), respBody, ARRAY_SIZE(respBody),200); 
	
	disconnect();
	free(jsonString);
	free(urlPath);
	free(header);
	
	return responseCode;
}

int evt_GetLastAction(char*apikey, Action * action)
{
	connect();
	
	char * urlPath = createGetLastActionURLPath(action->thng);
	char * header = createHeader(apikey);
	
	int responseCode = HTTP_Get(socket, EVRYTHNG_API_HOST, urlPath, header, respHeader, ARRAY_SIZE(respHeader), respBody, ARRAY_SIZE(respBody),200);
	
	if (responseCode == 200)
	{
		parseAction(action, respBody);
	}

	disconnect();
	free(urlPath);
	free(header);
	
	return responseCode;	
}
