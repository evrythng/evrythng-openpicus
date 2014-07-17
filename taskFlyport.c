#include "taskFlyport.h"
#include "evrythng.h"
#include "timestamp.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define API_KEY "rHzsaxMsshF1Di5MwSo2BLRNM0jS0J1iZkKbZuwbBbWjVisLGSLnyRhT1hlkeryCBdmaKL9LlSw7KZyN"
#define THNG_ID "UdMhm6bWPB5wfkTeRApAXkyf"
#define PROPERTY_TO_READ "read"
#define PROPERTY_TO_UPDATE "update"

void testEvtReadPropertyValue()
{	
	PropertyValue property = evt_ReadPropertyValue(API_KEY, THNG_ID, PROPERTY_TO_READ);
	char expectedValue[]= "0";
	double expectedTimestamp = 1405590561039;
	
	if (property.timestamp == expectedTimestamp)
	{
		_dbgwrite("SUCCESS property read TIMESTAMP\r\n");		
	}
	else{
		_dbgwrite("FAILED property read TIMESTAMP\r\n");		
	}

	if (strcmp(property.value,expectedValue) == 0)
	{
		_dbgwrite("SUCCESS property read VALUE\r\n");		
	}
	else{
		_dbgwrite("FAILED property read VALUE\r\n");		
	}
}

void testEvtUpdatePropertyValue()
{
	PropertyValue property;
	property.value = "10";
	property.timestamp = atof(getTimeStamp());
	
	int result = evt_UpdatePropertyValue(API_KEY, THNG_ID, PROPERTY_TO_UPDATE, &property);
	
	if (result == 200)
	{
		_dbgwrite("SUCCESS property update");
	}
	else{
		_dbgwrite("FAILED property update");		
	}
}

void connectToWifi()
{
	vTaskDelay(20); 
    // Flyport connects to default network
    WFConnect(WF_DEFAULT);
    while(WFGetStat() != CONNECTED);
    vTaskDelay(25);
}

void FlyportTask()
{
	connectToWifi();
	
	/*
	testEvtUpdatePropertyValue();
	vTaskDelay(500);
	*/
	
	testEvtReadPropertyValue();
	vTaskDelay(500);
	
	while(1);
};
