EVRYTHNG API - OpenPicus Flyport integration.
=============================================================

With the EVRYTHNG API wrapper for Open Picus you can easily integrate the EVRYTHNG platform in your Open Picus applications!

## Installation ##
### Dependencies ###

You need to add the provided libraries to your development environment.

1. JSON parser library (based on http://wiki.openpicus.com/index.php/JSON_Parser): 
    2. cJSON.h
    3. cJSON.c
    4. heap.s 
2. HTTP library (version 2.0 http://wiki.openpicus.com/index.php/HTTP):
    3. HTTPlib.h
    4. HTTPlib.c 
3. Timestamp library:
    4. timestamp.h
    5. timestamp.c
4. Internal utils:
    5. evrythng_utils.h
    6. evrythng_utils.c

### Library files ###

You need to add the wrapper files to your development environment:

 1. EVRYTHNG wrapper:
     2. evrythng.h
     3. evrythng.c
 
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
	
if (responseCode != 200)
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
	
if (responseCode != 200)
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

if (responseCode != 201)
{
    //Action was posted successfully
}
else
{
    //Check response code for error
}
```