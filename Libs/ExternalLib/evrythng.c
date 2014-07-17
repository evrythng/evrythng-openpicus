#include "taskFlyport.h"
#include "evrythng.h"
#include <stdio.h>
#include "HTTPlib.h"
#include "cJSON.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>

char buff[300];

TCP_SOCKET socket = INVALID_SOCKET;

char respHeader[500];
char respBody[500];

PropertyValue property;

char *print_number_value(cJSON *item)
{
	char *str;
	double d=item->valuedouble;
	if (fabs(((double)item->valueint)-d)<=DBL_EPSILON && d<=INT_MAX && d>=INT_MIN)
	{
		str=(char*)malloc(21);	/* 2^64+1 can be represented in 21 chars. */
		if (str) sprintf(str,"%d",item->valueint);
	}
	else
	{
		str=(char*)malloc(64);	/* This is a nice tradeoff. */
		if (str)
		{
			if (fabs(floor(d)-d)<=DBL_EPSILON){
				sprintf(str,"%.0f",d);
				_dbgwrite("0f:");
				_dbgwrite(str);
				_dbgwrite("\n\r");
			}
			else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9)
			{
				sprintf(str,"%e",d);
				_dbgwrite("e:");
				_dbgwrite(str);
				_dbgwrite("\n\r");

			}
			else{
				sprintf(str,"%f",d);
				_dbgwrite("f:");
				_dbgwrite(str);
				_dbgwrite("\n\r");

			}
		}
	}
	return str;
}

static char* cJSON_strdup(const char* str)
{
      size_t len;
      char* copy;

      len = strlen(str) + 1;
      if (!(copy = (char*)malloc(len))) return 0;
      memcpy(copy,str,len);
      return copy;
}

/* Render the cstring provided to an escaped version that can be printed. */
static char *print_string_ptr(const char *str)
{
	const char *ptr;char *ptr2,*outp;int len=0;unsigned char token;
	
	if (!str) return cJSON_strdup("");
	ptr=str;
	
	while ((token=*ptr) && ++len) 
	{
		if (strchr("\"\\\b\f\n\r\t",token)) len++; 
		else if (token<32) 
			len+=5;
			ptr++;
	}
	
	outp=(char*)malloc(len+3);
	if (!outp) return 0;

	ptr2=outp;
	ptr=str;
	//*ptr2++='\"';
	while (*ptr)
	{
		if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\'){
			*ptr2++=*ptr++;
		}
		else
		{
			*ptr2++='\\';
			switch (token=*ptr++)
			{
				case '\\':	*ptr2++='\\';	break;
				case '\"':	*ptr2++='\"';	break;
				case '\b':	*ptr2++='b';	break;
				case '\f':	*ptr2++='f';	break;
				case '\n':	*ptr2++='n';	break;
				case '\r':	*ptr2++='r';	break;
				case '\t':	*ptr2++='t';	break;
				default: sprintf(ptr2,"u%04x",token);ptr2+=5;	break;	/* escape and print */
			}
		}
	}
	//*ptr2++='\"';
	*ptr2++=0;
	return outp;
}
/* Invote print_string_ptr (which is useful) on an item. */
static char *print_string(cJSON *item)	{return print_string_ptr(item->valuestring);}


char* findJsonInResponseStr(char* str) {
	size_t pos;
	size_t iter;
	char jsonChars[4] = {'{', '[', '}', ']'};
	unsigned short int foundStart = FALSE;
	unsigned short int charType = 0;
	unsigned short int matchChar;
	int startPos;

	for (pos = 0; pos < strlen(str); pos++) {
		if(foundStart == TRUE) {
			if (str[pos] == jsonChars[matchChar + charType]) {
				char *subbuff = malloc(pos - startPos + 1 + 1);
				memcpy(subbuff, &str[startPos], pos - startPos + 1);
				subbuff[pos + 1] = '\0';
				return subbuff;
			}
		} else {
			for (iter = 0; iter < 2; iter++) {
				if (str[pos] == jsonChars[iter + charType]) {
					charType = 2;
					foundStart = TRUE;
					startPos = pos;
					matchChar = iter;
				}
			}
		}
		
	}
	return NULL;
}

/**
* Internal utils
*/
static void connect()
{
	socket = TCPClientOpen(EVRYTHNG_API_HOST, EVRYTHNG_API_PORT);	
	while(!TCPisConn(socket))
    {
		vTaskDelay(15);
		_dbgwrite(".");
    }
	_dbgwrite("\r\nTCP connection OK\r\n");
}

static void disconnect()
{
	TCPClientClose(socket);
}

/**
* EVT API
*/
void createPropertyURL(char* url, char* thngId, char* propertyName){	
	sprintf(url, "%s%s%s%s%s", THNGS_PATH, thngId, PROPERTIES_PATH, propertyName, FROM_LATEST);	
};

void createUpdatePropertyURL(char* url, char* thngId, char* propertyName){
	sprintf(url, "%s%s%s%s", THNGS_PATH, thngId, PROPERTIES_PATH, propertyName);	
};

void createPostActionURL(char* url, char* pathData, char* actionType){
	sprintf(url, "%s%s%s", ACTIONS_PATH, "/", actionType);
}

PropertyValue evt_ReadPropertyValue(char* apikey, char* thngId, char* propertyName)
{
	connect();

	char pathData[150];	
	createPropertyURL(pathData, thngId, propertyName);
	
	int responseCode = HTTP_Get(socket, EVRYTHNG_API_HOST, pathData, "Authorization: rHzsaxMsshF1Di5MwSo2BLRNM0jS0J1iZkKbZuwbBbWjVisLGSLnyRhT1hlkeryCBdmaKL9LlSw7KZyN", respHeader, ARRAY_SIZE(respHeader), respBody, ARRAY_SIZE(respBody),200);
	
	_dbgwrite("\n\rResponse Header GET:\n\r");
	_dbgwrite(respHeader);
	_dbgwrite("\n\rResponse Body GET:\n\r");
	_dbgwrite(respBody);
	_dbgwrite("\n\r");
	
	cJSON *json = cJSON_Parse(findJsonInResponseStr(respBody));	
	
	if (!json){
        char *error = (char*)cJSON_GetErrorPtr();
        _dbgwrite("\n\r An error was encountered\n\r");
        _dbgwrite(error);
    }
	else{
		cJSON *item = cJSON_GetArrayItem(json,0);
		cJSON *timestamp = cJSON_GetObjectItem(item,"timestamp");
		cJSON *value = cJSON_GetObjectItem(item,"value");
		
		property.timestamp = timestamp->valuedouble;
		property.value = print_string_ptr(value->valuestring);
	}	
	
	cJSON_Delete(json);	
	disconnect();
	
	return property;	
};

int evt_UpdatePropertyValue(char* apikey, char* thngId, char* propertyName, PropertyValue* property)
{
	connect();

	char pathData[150];	
	createUpdatePropertyURL(pathData, thngId, propertyName);
	
	/*
	[
		{
			"timestamp": 1404184104538,
			"value": "13"
		}
	]
	*/
	cJSON * jsonData = cJSON_CreateArray();
	cJSON * jsonProperty = cJSON_CreateObject();
	
    cJSON_AddItemToObject(jsonProperty,"value",cJSON_CreateString(property->value));
    cJSON_AddItemToObject(jsonProperty,"timestamp",cJSON_CreateNumber(property->timestamp));

    cJSON_AddItemToArray(jsonData,jsonProperty);

    char *s_out = cJSON_PrintUnformatted(jsonData);

	_dbgwrite("\n\rJSON:\n\r");
	_dbgwrite(s_out);
	_dbgwrite("\n\r");

	int responseCode = HTTP_Put(socket, EVRYTHNG_API_HOST, pathData, "Authorization: rHzsaxMsshF1Di5MwSo2BLRNM0jS0J1iZkKbZuwbBbWjVisLGSLnyRhT1hlkeryCBdmaKL9LlSw7KZyN\r\nContent-Type: application/json\r\n", s_out, respHeader, ARRAY_SIZE(respHeader), respBody, ARRAY_SIZE(respBody),200);
			
	_dbgwrite("\n\rResponse Header PUT:\n\r");
	_dbgwrite(respHeader);
	_dbgwrite("\n\rResponse Body PUT:\n\r");
	_dbgwrite(respBody);
	_dbgwrite("\n\r");
	
	disconnect();
	free(s_out);
	
	return responseCode;	
};

int evt_PostAction(char* apikey, char* actionType, char* thingId)
{

/*
?createThng=<Boolean> (applies only for thng actions, default: false, access private)
{
--*"type": <ActionType>,
--*"locationSource": "sensor"|"place"|"geoIp"|"unknown",
--"timestamp": <timestamp>,
--"location": {
----"place": <Ref>,
----"position": <GeoJSON:Point>,
----"latitude": <Double>, // (deprecated)
----"longitude": <Double> // (deprecated)
--},
--"device": <String>,
--"context": {},
--"customFields":<CustomFields>
}
*/
	connect();

	char pathData[150];
	createPostActionURL(pathData, actionType);
	
	cJSON * jsonData = cJSON_CreateObject();
	
    cJSON_AddItemToObject(jsonProperty,"type",cJSON_CreateString(property->value));
    cJSON_AddItemToObject(jsonProperty,"locationSource",cJSON_CreateString(property->timestamp));

    cJSON_AddItemToArray(jsonData,jsonProperty);

    char *s_out = cJSON_PrintUnformatted(jsonData);

	_dbgwrite("\n\rJSON:\n\r");
	_dbgwrite(s_out);
	_dbgwrite("\n\r");

	int responseCode = HTTP_Put(socket, EVRYTHNG_API_HOST, pathData, "Authorization: rHzsaxMsshF1Di5MwSo2BLRNM0jS0J1iZkKbZuwbBbWjVisLGSLnyRhT1hlkeryCBdmaKL9LlSw7KZyN\r\nContent-Type: application/json\r\n", s_out, respHeader, ARRAY_SIZE(respHeader), respBody, ARRAY_SIZE(respBody),200);
			
	
	
	
	
	disconnect();
	free(s_out);
	return responseCode;
}