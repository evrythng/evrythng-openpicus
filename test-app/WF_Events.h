#include "GenericTypeDefs.h"

#include "TCPIP Stack/TCPIP.h"



void OnConnectionSuccessful(UINT8 event , UINT16 eventinfo);
void OnConnectionFailed(UINT8 event , UINT16 eventinfo);
void OnConnectionTempLost(UINT8 event , UINT16 eventinfo);
void OnConnectionPermLost(UINT8 event , UINT16 eventinfo);
void OnScanResult(UINT8 event , UINT16 eventinfo);
void OnConnectionReestablished(UINT8 event , UINT16 eventinfo);
