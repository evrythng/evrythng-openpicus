#include "timestamp.h"
#include "taskFlyport.h"
#include <time.h>

char epochString[50];
DWORD epoch=0;
DWORD epochtime=0xA2C2A;

time_t now;
struct tm *ts;
struct tm mytime;

// to properly set GMT by adding or removing
// the hours for GMT zone (for example Rome = +1 or +2 GMT)
// negative values are supported too...
int GMT_hour_adding = 0;

void initializeTime(int GMT_zone)
{  
  GMT_hour_adding = GMT_zone;
	
  while(epoch<epochtime)
  {
    vTaskDelay(50);
    //Remember to enable the SNTP Client in the wizard.
    epoch=SNTPGetUTCSeconds();
  }

  epoch=SNTPGetUTCSeconds();
  now=(time_t)epoch;
  ts=localtime(&now);
  vTaskDelay(20);
  ts->tm_hour = (ts->tm_hour + GMT_hour_adding);
  // Correct if overflowed hour 0-24 format
  if(ts->tm_hour > 24)
  {
    ts->tm_hour = ts->tm_hour - 24;
  }
  else if(ts->tm_hour < 0)
  {
    ts->tm_hour = ts->tm_hour +24;
  }	  
  sprintf (epochString, "\nReceived date/time is: %s \r\n", asctime (ts) );
  UARTWrite(1,epochString);
  RTCCSet(ts); //Set time
};

char* getTimeStamp()
{        
	 RTCCGet(&mytime);//get actual data/time
	 time_t timeSinceEpoch = mktime(&mytime);
	 sprintf(epochString, "%ld000", timeSinceEpoch);
	 return epochString;
};
