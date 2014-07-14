#include "taskFlyport.h"
#include "evrythng.h"

#include <stdio.h>
#include <string.h>

void testEvtGetPropertyValue()
{
	const char* apiKey = "InCChEsfCIrbM4pwkEi31jipNSK1y95HKoTwiaNlIkrVqgUiVKlr293nggUT1i0fiWSyKjUeczaNPV7X";
	const char* thngId = "5368ee7ce4b0eec2cc7a41d8";
	const char* propertyName = "output";

	PropertyValue propertyValue = evtGetPropertyValue(apiKey,thngId,propertyName);
	
	char expectedPropertyValue[20];
	strcpy(expectedPropertyValue,"600.00");
	if(strcmp(expectedPropertyValue,propertyValue.value) == 0)
	{
		_dbgwrite("\r\n SUCCESS property value OK - testCreateReadPropertyURL\r\n");
	}
	else {
		_dbgwrite("\r\n FAILED propety value ERR - testCreateReadPropertyURL\r\n");
		_dbgwrite(propertyValue.value);
	}	
	
	char expectedTimeStamp[20];
	strcpy(expectedTimeStamp,"1405095589239");	
	if(strcmp(expectedTimeStamp,propertyValue.timestamp) == 0)
	{
		_dbgwrite("\r\n SUCCESS timestamp value OK - testCreateReadPropertyURL\r\n");
	}
	else {
		_dbgwrite("\r\n FAILED timestamp value ERR - testCreateReadPropertyURL\r\n");
		_dbgwrite(propertyValue.timestamp);
	}		
};

/*
void testCreateReadPropertyURL()
{
	char* thngId = "5368ee7ce4b0eec2cc7a41d8";
	char* propertyName = "output";
	
	char expectedResult[150];
	strcpy(expectedResult,"/thngs/5368ee7ce4b0eec2cc7a41d8/properties/output?from=latest");
	
	char url[150];
	createReadPropertyURL(url, thngId,propertyName);

	if(strcmp(expectedResult,url) == 0)
	{
		_dbgwrite("\r\n SUCCESS testCreateReadPropertyURL\r\n");
	}
	else {
		_dbgwrite("\r\n FAILED testCreateReadPropertyURL\r\n");
	}	
}
*/

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
	testCreateReadPropertyURL();
	vTaskDelay(500);
	*/
	
	testEvtGetPropertyValue();
	
	while(1);
};
