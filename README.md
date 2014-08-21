EVRYTHNG API - OpenPicus Flyport Wrapper
=============================================================

The EVRYTHNG API wrapper for Open Picus makes it easier to integrate some of the basic functionality of the EVRYTHNG platform in your Open Picus applications!

## Installation ##

To use the wrapper, add the content of openpicus-evrythng-libs to your project as external libraries.

### Services Selection ###

From the Open Picus Services (configured through the network wizard in the IDE) you must include at least the following:
- DHCP Client
- RTCC library
- SNTP Client


### SSL ###

The library uses SSL to communicate with the EVRYTHNG API. Remember to configure your Open Picus project to support SSL. [SSL Configuration](http://wiki.openpicus.com/index.php/Secure_Sockets_Layer_(SSL))

### Included libraries ###

- JSON parser library (based on http://wiki.openpicus.com/index.php/JSON_Parser): 
    - cJSON.h
    - cJSON.c
    - heap.s 
- HTTP library (version 2.0 http://wiki.openpicus.com/index.php/HTTP):
    - HTTPlib.h
    - HTTPlib.c 
- Timestamp library:
    - timestamp.h
    - timestamp.c
- Internal utils:
    - evrythng_utils.h
    - evrythng_utils.c
- EVRYTHNG wrapper:
     - evrythng.h
     - evrythng.c
 
----------
## Usage ##

The EVRYTHNG wrapper for Open Picus is based on a set of structures and functions that let you invoke the EVRYTHNG engine API easily from the Open Picus code.

More info about the full API and response codes can be found here: https://dev.evrythng.com/documentation/api

Currently we support the following functionality out of the box:

### Get Properties of Thngs ###
Retrieves the last update of a property value of a Thng.
```c
Property property;
	
int responseCode = evt_GetPropertyValue("YOUR API KEY", "YOUR THNG ID", "YOUR PROPERTY NAME", &property);
	
if (responseCode == 200)
{
    //Property was read successfully, values can be read from the property struct
}
else
{
    //Check response code for error
}
```

### Update Properties of Thngs ###
Updates a property value of a Thng.
```c
Property property;
property.value = "10";
property.timestamp = atof(getTimeStamp());
	
int responseCode = evt_UpdatePropertyValue("YOUR API KEY", "YOUR THNG ID", "YOUR PROPERTY NAME", &property);
	
if (responseCode == 200)
{
    //Property was updated successfully
}
else
{
    //Check response code for error
}
```
### Post actions on Thngs ###
For predefined actions, you can use:

 - ACTION_CHECKINS
 - ACTION_SCANS
 - ACTION_IMPLICIT_SCANS

to set the predefined action types or use your own for custom actions.

```c
Action checkinAction;
//
checkinAction.type = ACTION_CHECKINS;
checkinAction.thng = "YOUR THNG ID";

int responseCode = evt_PostAction("YOUR API KEY", &checkinAction);

if (responseCode == 201)
{
    //Action was posted successfully
}
else
{
    //Check response code for error
}
```
### Get last action on Thngs ###
Retrieves the last action performed on a thng.

```c
Action lastAction;
lastAction.thng = "YOUR THNG ID";
	
int responseCode = evt_GetLastAction(API_KEY, &lastAction);

if (responseCode == 200)
{
     //Action was read successfully, values can be read from the action struct
}
else
{
    //Check response code for error
}
```

----------
## Test application ##

You can find a test application on how to use the functionality of the wrapper in the test-app folder.
