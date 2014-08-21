#include "evrythng_utils.h"
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

static void findJsonInResponseStr(char* jsonInResponse, const char* str) {
	size_t pos;
	size_t beginCharsIndex;
	size_t endCharsIndex;
	
	char beginChars[2] = {'{', '['};
	char endChars[2] = {'}', ']'};
	
	unsigned short int foundStart = FALSE;
	unsigned short int foundEnd = FALSE;
	
	int trimStart;
	int trimEnd;
		
	//search start pos
	for (pos = 0; pos < strlen(str); pos++) {
		for(beginCharsIndex=0; beginCharsIndex < strlen(beginChars); beginCharsIndex++){
			if (str[pos] == beginChars[beginCharsIndex]) {
				trimStart = pos;
				foundStart = TRUE;
				break;
			}
		}
		if (foundStart) break;
	}
	
	//search end pos
	for (pos = strlen(str)-1; pos > 0; pos--) {	
		for(endCharsIndex=0; endCharsIndex < strlen(endChars); endCharsIndex++){
			if (str[pos] == endChars[endCharsIndex]) {
				trimEnd = pos;
				foundEnd = TRUE;
				break;
			}
		}
		if (foundEnd) break;
	}
		
	if (foundStart && foundEnd){
		int length = trimEnd - trimStart + 1;		
		strncpy(jsonInResponse, &str[trimStart], length);
		jsonInResponse[length] = '\0';
	}   
}

static char*cJSON_strdup(const char*str)                                                                          
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

void parseProperty(Property * property, char* response)
{
	char jsonInResponse[strlen(response)];
	findJsonInResponseStr(jsonInResponse, response);
	cJSON *json = cJSON_Parse(jsonInResponse);	
	if (!json){
		char *error = (char*)cJSON_GetErrorPtr();
		_dbgwrite("\n\r An error was encountered\n\r");
		_dbgwrite(error);
	}
	else{
		cJSON *item = cJSON_GetArrayItem(json,0);
		cJSON *timestamp = cJSON_GetObjectItem(item,"timestamp");
		cJSON *value = cJSON_GetObjectItem(item,"value");
			
		property->timestamp = timestamp->valuedouble;
		property->value = print_string_ptr(value->valuestring);
	}	
	
	cJSON_Delete(json);
}

char * serializeProperty(Property * property)
{
	cJSON * jsonData = cJSON_CreateArray();
	cJSON * jsonProperty = cJSON_CreateObject();	
    cJSON_AddItemToObject(jsonProperty,"value",cJSON_CreateString(property->value));
    cJSON_AddItemToObject(jsonProperty,"timestamp",cJSON_CreateNumber(property->timestamp));
    cJSON_AddItemToArray(jsonData,jsonProperty);
    return cJSON_PrintUnformatted(jsonData);
}

void parseAction(Action * action, char* response){

	char jsonInResponse[strlen(response)];
	findJsonInResponseStr(jsonInResponse, response);
	//char jsonInResponse2[500] = "[{\"id\":\"Ud9ePatw8BKaFGbmcsMYhtYk\",\"createdAt\":1408557296116,\"timestamp\":1408557296116,\"type\":\"checkins\",\"user\":\"UAteE6yp8VpRNbwFhkFN8hHp\",\"location\":{\"latitude\":51.5142,\"longitude\":-0.0931,\"position\":{\"type\":\"Point\",\"coordinates\":[-0.0931,51.5142]}},\"locationSource\":\"geoIp\",\"thng\":\"UUQBkCsyPBKa2GSGys6hErWf\",\"product\":\"UdQehq9ePVKRkmbmyPMYENdg\"}]";
	
	cJSON *json = cJSON_Parse(jsonInResponse);	
	if (!json){
		char *error = (char*)cJSON_GetErrorPtr();
		_dbgwrite("\n\r An error was encountered: ");
		_dbgwrite(error);
	}
	else{
		cJSON *item = cJSON_GetArrayItem(json,0);
		cJSON *actionType = cJSON_GetObjectItem(item,"type");
		cJSON *thng = cJSON_GetObjectItem(item,"thng");
		cJSON *locationSource = cJSON_GetObjectItem(item,"locationSource");
		
		action->type = print_string_ptr(actionType->valuestring);
		action->thng = print_string_ptr(thng->valuestring);
		action->locationSource = print_string_ptr(locationSource->valuestring);
	}	
	
	cJSON_Delete(json);
}

char * serializeAction(Action * action)
{
	cJSON * jsonData = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonData,"type",cJSON_CreateString(action->type));
	cJSON_AddItemToObject(jsonData,"thng",cJSON_CreateString(action->thng));
    return cJSON_PrintUnformatted(jsonData);
}
