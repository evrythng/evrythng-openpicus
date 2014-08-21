#ifndef __EVRYTHNG_API_H_
#define __EVRYTHNG_API_H_

#define EVRYTHNG_API_HOST "api-test.evrythng.net" 
#define EVRYTHNG_API_PORT "443"

#define THNGS_PATH "/thngs/"
#define PROPERTIES_PATH "/properties/"
#define FROM_LATEST "?from=latest"
#define AUTHORIZATION_HEADER_KEY "Authorization: "
#define CONTENT_TYPE_HEADER_KEY "Content-Type: "
#define ACTIONS_PATH "/actions/"
#define LAST_ACTION_PATH "/actions/all?from=latest&perPage=1&thng="

#define ACTION_SCANS "scans"
#define ACTION_IMPLICIT_SCANS "implicitScans"
#define ACTION_CHECKINS "checkins"

#define LOCATION_TYPE_SENSOR "sensor"
#define LOCATION_TYPE_PLACE "place"
#define LOCATION_TYPE_GEOIP "geoIp"
#define LOCATION_TYPE_UNKNOWN "unknown"

typedef struct {
	long double timestamp;
	char* value;
} Property;

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
typedef struct {
	char* type;
	char* locationSource;
	char* thng;
} Action;

char * createHeader(char*apikey);
char * createHeader_ContentType(char*apikey, char*contentType);

char * createGetPropertyURLPath(char* thngId, char* propertyName);
char * createUpdatePropertyURLPath(char* thngId, char* propertyName);
char * createPostActionURLPath(char* actionType);

int evt_GetPropertyValue(char* apikey, char* thngId, char* propertyName, Property * value);
int evt_UpdatePropertyValue(char* apikey, char* thngId, char* propertyName, Property * value);
int evt_PostAction(char* apikey, Action * action);
int evt_GetLastAction(char*apikey, Action * action);

#endif /* __EVRYTHNG_API_H_ */
