#ifndef __EVRYTHNG_API_H_
#define __EVRYTHNG_API_H_

#define EVRYTHNG_API_HOST "api.evrythng.com" 
#define EVRYTHNG_API_PORT "80"

#define THNGS_PATH "/thngs/"
#define PROPERTIES_PATH "/properties/"
#define FROM_LATEST "?from=latest"

#define ACTIONS_PATH "/actions/"

typedef struct {
	double timestamp;
	char* value;
} PropertyValue;

#define SCANS 0
#define IMPLICIT_SCANS 1
#define CHECKINS 2

static const char * actionTypes[] = {
    "scans",
    "implicitScans",
    "checkins"
};

void createPropertyURL(char* url, char* thngId, char* propertyName);
void createUpdatePropertyURL(char* url, char* thngId, char* propertyName);

PropertyValue evt_ReadPropertyValue(char* apikey, char* thngId, char* propertyName);
int evt_UpdatePropertyValue(char* apikey, char* thngId, char* propertyName, PropertyValue * value);

Action evt_PostAction(char* apikey, char* actionType, char* thingId);

#endif /* __EVRYTHNG_API_H_ */
