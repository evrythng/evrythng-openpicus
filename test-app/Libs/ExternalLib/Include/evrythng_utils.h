#ifndef __EVRYTHNG_UTILS_H_
#define __EVRYTHNG_UTILS_H_

#include "evrythng.h"

void parseProperty(Property * property, char* response);
void parseAction(Action * action, char* response);
void findJsonInResponseStr(char * jsonInResponse, const char* str);

char * serializeProperty(Property * property);
char * serializeAction(Action * action);

#endif
