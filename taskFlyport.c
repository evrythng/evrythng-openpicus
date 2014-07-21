#include "taskFlyport.h"
#include "evrythng.h"
#include "timestamp.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define API_KEY "YOUR API KEY"
#define THNG_ID "YOUR THNG ID"
#define PROPERTY_TO_READ "read"
#define PROPERTY_TO_UPDATE "update"

void test_evt_GetPropertyValue()
{	
	Property property;
	int responseCode = evt_GetPropertyValue(API_KEY, THNG_ID, PROPERTY_TO_READ,&property);
	
	if (responseCode != 200)
	{
		_dbgwrite("FAILED property read Server Error\r\n");				
	}
	
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

void test_evt_UpdatePropertyValue()
{
	Property property;
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

void test_evt_PostAction(){
	Action checkinAction;	
	checkinAction.type = ACTION_CHECKINS;
	checkinAction.thng = THNG_ID;
	
	int result = evt_PostAction(API_KEY, &checkinAction);

	if (result == 201)
	{
		_dbgwrite("SUCCESS testEvtPostAction\r\n");
	}
	else{
		_dbgwrite("FAILED testEvtPostAction\r\n");		
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
	
	test_evt_GetPropertyValue();
	vTaskDelay(500);
	
	test_evt_UpdatePropertyValue();
	vTaskDelay(500);
	
	test_evt_PostAction();
	vTaskDelay(500);

	while(1);
};

