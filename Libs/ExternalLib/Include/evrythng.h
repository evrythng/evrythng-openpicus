#ifndef __EVRYTHNG_API_H_
#define __EVRYTHNG_API_H_

#define EVRYTHNG_API_HOST "api.evrythng.com" 
#define EVRYTHNG_API_PORT "80"

#define THNGS_PATH "/thngs/"
#define PROPERTIES_PATH "/properties/"
#define FROM_LATEST "?from=latest"

typedef struct {
	char* timestamp;
	char* value;
} PropertyValue;

void createReadPropertyURL(char* url, char* thngId, char* propertyName);

PropertyValue evtGetPropertyValue(char* apikey, char* thngId, char* propertyName);

#endif /* __EVRYTHNG_API_H_ */
