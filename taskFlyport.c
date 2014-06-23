#include "taskFlyport.h"
#include "grovelib.h"
#include "analog_temp.h"

// EVRYTHNG Operator API Token
char ACCESS_TOKEN[] = "YOUR_OPERATOR_EVRYTHNG_API_KEY_HERE";

void writeProperty(char * payload, char * propertyName, char* thngId)
{
	// TCP	
	TCP_SOCKET sock = INVALID_SOCKET;
	char msg[1500];
	int cnt = 0;
	BOOL flagErr = FALSE;
	
	char bff[250];
	int RxLen=0;
	int LenTOT=0;

	sock = TCPClientOpen("api.evrythng.com","80");
	 
	while(!TCPisConn(sock))
	{
		if(cnt==10)
		{
			flagErr = TRUE;
			break;
		}
		vTaskDelay(50);
		cnt++;
  }
 
	if(flagErr)
	{
		UARTWrite(1,"\r\nTimeout error...\r\n");
		sock=INVALID_SOCKET;
	}
	 
	UARTWrite(1,"\r\nFlyport is ready to send data to the EVRYTHNG API\r\n");
 
	if(TCPisConn(sock))
	{
		// Send measurements
		sprintf(msg,"POST /thngs/%s/properties/%s?access_token=%s HTTP/1.1\r\nHost: api.evrythng.com\r\nAccept: application/json\r\n\Content-Type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n", 
		thngId, propertyName, ACCESS_TOKEN, strlen(payload), payload);
		UARTWrite(1,"\r\nRequest:\r\n");
		UARTWrite(1,msg);
		vTaskDelay(100);
		TCPWrite(sock,msg,strlen(msg));
		UARTWrite(1,"Measurements sent!");
		
		// And close the connection
		TCPClientClose(sock);
		vTaskDelay(100);
	}

};

void FlyportTask()
{
	// Config	
	const char* propertyLight = "output";
	const char* propertyTemp = "temperature";
	const char* thngId = "5368ee7ce4b0eec2cc7a41d8";
	
	UARTWrite(1,"Flyport Task is starting...\r\n");
	
	// Grove
	void *board = new (GroveNest);
 
	// GROVE devices
	// Light sensor
	void *lightSensor = new (An_i);
	attachToBoard(board, lightSensor, AN3);
	// Temperature sensor
	void *tempAn = new(An_Temp);
	attachToBoard(board, tempAn, AN1);
	 
	//Flyport connects to default network
	WFConnect(WF_DEFAULT);
	while(WFGetStat() != CONNECTED);
	while(!DHCPAssigned);
	vTaskDelay(25);
	UARTWrite(1,"Flyport Wi-fi G connected... to network!\r\n");
	 
	  
	char payloadTemp[100];
	char payloadLight[100];
		
	// and send!
	while(1) {
		// Light sensor
		sprintf(payloadLight, "[{\"value\":%3.2f}]", (double)get(lightSensor));
		writeProperty(payloadLight, propertyLight, thngId);
		
		vTaskDelay(100);
		
		// Temperature sensor
		sprintf(payloadTemp, "[{\"value\":%3.2f}]", (double)get(tempAn));
		// then print to UART1
		writeProperty(payloadTemp, propertyTemp, thngId);
		
		vTaskDelay(100);
	}
}
