#ifndef __EVRYTHNG_UTILS_H_
#define __EVRYTHNG_UTILS_H_

#include "evrythng.h"

char * serializeProperty(Property * property);
void parseProperty(Property * property, char* response);

char * serializeAction(Action * action);
void parseAction(Action * action, char* response);

#endif
