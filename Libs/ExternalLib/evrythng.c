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

TCP_SOCKET client = INVALID_SOCKET;

char respHeader[500];
char respBody[500];

PropertyValue property;

void createReadPropertyURL(char* url, char* thngId, char* propertyName){	
	sprintf(url, "%s%s%s%s%s", THNGS_PATH, thngId, PROPERTIES_PATH, propertyName, FROM_LATEST);	
};

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
			if (fabs(floor(d)-d)<=DBL_EPSILON)			sprintf(str,"%.0f",d);
			else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9)	sprintf(str,"%e",d);
			else										sprintf(str,"%f",d);
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
	ptr=str;while ((token=*ptr) && ++len) {if (strchr("\"\\\b\f\n\r\t",token)) len++; else if (token<32) len+=5;ptr++;}
	
	outp=(char*)malloc(len+3);
	if (!outp) return 0;

	ptr2=outp;ptr=str;
	//*ptr2++='\"';
	while (*ptr)
	{
		if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\') *ptr2++=*ptr++;
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

PropertyValue evtGetPropertyValue(char* apikey, char* thngId, char* propertyName)
{
	client = TCPClientOpen(EVRYTHNG_API_HOST, EVRYTHNG_API_PORT);	
	while(!TCPisConn(client))
    {
		vTaskDelay(15);
		_dbgwrite(".");
    }
	_dbgwrite("TCP connection OK\r\n");

	char pathData[150];	
	createReadPropertyURL(pathData, thngId, propertyName);
	
	int responseCode = HTTP_Get(client, EVRYTHNG_API_HOST, pathData, "Authorization: InCChEsfCIrbM4pwkEi31jipNSK1y95HKoTwiaNlIkrVqgUiVKlr293nggUT1i0fiWSyKjUeczaNPV7X", respHeader, ARRAY_SIZE(respHeader), respBody, ARRAY_SIZE(respBody),200);
	
	_dbgwrite("\n\rResponse Header:\n\r");
	_dbgwrite(respHeader);
	_dbgwrite("\n\rResponse Body:\n\r");
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

		property.timestamp = print_number_value(timestamp);
		property.value = print_string(value);	
	}		
	cJSON_Delete(json);
	
	return property;	
};
