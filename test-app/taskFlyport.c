#include "taskFlyport.h"
#include "evrythng.h"
#include "timestamp.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

//test env conf
#define API_KEY "Y2LQJ4i0PlGXP6cmQnDiODDhKmLdrr5wsVxmHiz1RWLpSBHAN6wo382w5NX9nYxSxxLOoPxN2gx2UlLt"
#define THNG_ID "UUQBkCsyPBKa2GSGys6hErWf"
#define PROPERTY_TO_READ "read"
#define PROPERTY_TO_UPDATE "update"

const char EXPECTED_PROPERTY_VALUE [] = "10";
long double EXPECTED_PROPERTY_TIMESTAMP = 1408534003255;

#define TEST_NUMBER 4
char testResults[TEST_NUMBER][150];
int testNumber = 0;

/**
* TEST UTILITIES
*/
void before(char* testName)
{
	testNumber++;
	char message[80];
	sprintf(message, "TEST %d - %s\r\n", testNumber, testName);	
	sprintf(testResults[testNumber-1],message);
}
void success(char* text)
{                                                                                                                
	char message[80];
	sprintf(message, "TEST %d PASSED: %s\r\n", testNumber, text);
	strcat(testResults[testNumber-1],message);
}
void fail(char* text)
{
	char message[80];
	sprintf(message, "TEST %d FAILED: %s\r\n", testNumber, text);
	strcat(testResults[testNumber-1],message);	
}
void assertEqualsString(char* message, char* expected, char * actual)
{
	if (strcmp(actual,expected) == 0)
	{
		success(message);		
	}
	else{
		char failMessage[50];
		sprintf(failMessage, "Value mismatch, expected %s , found %s", expected, actual);		
		fail(failMessage);				
	}
}
void assertEqualsInteger(char*message, int expected, int actual)
{
	if (expected != actual)
	{
		char message[50];
		sprintf(message, "Unexpected response code, expected %d , found %d", expected, actual);
		fail(message);				
	}
	else{
		success(message);				
	}
}
void assertEqualsDouble(char*message, long double expected, long double actual)
{
	if (expected != actual)
	{
		char message[50];
		sprintf(message, "Unexpected response code, expected %Lf , found %Lf", expected, actual);
		fail(message);				
	}
	else{
		success(message);				
	}
}
void printResults()
{
	int i;
	for (i = 0; i < TEST_NUMBER; i++)
	{
		_dbgwrite(testResults[i]);
		_dbgwrite("\r\n");
	}
}

/**
*	TEST CASES
*/

void test_evt_GetPropertyValue()
{
	before("GetPropertyValue");
	
	Property property;
	int responseCode = evt_GetPropertyValue(API_KEY, THNG_ID, PROPERTY_TO_READ,&property);
	
	assertEqualsInteger("Response code OK", 200, responseCode);
	assertEqualsDouble("Timestamp OK", EXPECTED_PROPERTY_TIMESTAMP, property.timestamp);
	assertEqualsString("Value OK", EXPECTED_PROPERTY_VALUE, property.value);	
}

void test_evt_UpdatePropertyValue()
{
	before("UpdatePropetyValue");
	
	Property property;
	property.value = "10";
	property.timestamp = atof(getTimeStamp());
	
	int responseCode = evt_UpdatePropertyValue(API_KEY, THNG_ID, PROPERTY_TO_UPDATE, &property);
		
	assertEqualsInteger("Response code OK", 200, responseCode);	
}

void test_evt_PostAction(){
	
	before("PostAction");
	
	Action checkinAction;	
	checkinAction.type = ACTION_CHECKINS;
	checkinAction.thng = THNG_ID;
	
	int responseCode = evt_PostAction(API_KEY, &checkinAction);

	assertEqualsInteger("Response code OK", 201, responseCode);	
}

void test_evt_GetLastAction(){
	
	before("GetLastAction");
	
	Action lastAction;
	lastAction.thng = THNG_ID;
	
	int responseCode = evt_GetLastAction(API_KEY, &lastAction);

	assertEqualsInteger("Response code OK", 200, responseCode);
	assertEqualsString("ActionType OK", ACTION_CHECKINS, lastAction.type);
	assertEqualsString("Thng OK", THNG_ID, lastAction.thng);
	assertEqualsString("locationSource OK", LOCATION_TYPE_GEOIP, lastAction.locationSource);	
}

void test_findJsonInResponseStr()
{
	before("TrimResponse");
	
	char message[20] = "12{hello}0";
	char * messageTrimmed = findJsonInResponseStr(message);
	assertEqualsString("Message with curly braces OK", "{hello}", messageTrimmed);	
	
	char message2[20] = "12[{hello}]0";
	char * message2Trimmed = findJsonInResponseStr(message2);
	assertEqualsString("Message with brackets OK", "[{hello}]", message2Trimmed);	
	
	char message3[20] = "12[{he[l]lo}]0";
	char * message3Trimmed = findJsonInResponseStr(message3);
	assertEqualsString("Message with nested brackets OK", "[{he[l]lo}]", message3Trimmed);		
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

	_dbgwrite("\r\nSTARTING TESTS\r\n");
	
	test_evt_GetPropertyValue();
	vTaskDelay(500);
	
	test_evt_UpdatePropertyValue();
	vTaskDelay(500);
	
	test_evt_PostAction();
	vTaskDelay(500);

	test_evt_GetLastAction();
	vTaskDelay(500);

	_dbgwrite("\r\nTESTS FINISHED\r\n");
	
	printResults();
	
	while(1);
};

