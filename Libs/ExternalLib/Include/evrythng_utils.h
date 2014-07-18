#ifndef __EVRYTHNG_UTILS_H_
#define __EVRYTHNG_UTILS_H_

#include "evrythng.h"

void parseProperty(Property * property, char* response);
char* findJsonInResponseStr(char* str);

char * serializeProperty(Property * property);
char * serializeAction(Action * action);

#endif
