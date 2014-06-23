Sample code for the EVRYTHNG - OpenPicus Flyport integration.
=============================================================

A sample code to update EVRYTHNG properties from an OpenPicus Flyport over HTTP.

To make it work just edit the following in `taskflyport.c`:

```c
  // EVRYTHNG Operator API Token
  char ACCESS_TOKEN[] = "YOUR_OPERATOR_EVRYTHNG_API_KEY_HERE";  

  [...]

  // Config	
  const char* propertyLight = "output";
  const char* propertyTemp = "temperature";
  const char* thngId = "5368ee7ce4b0eec2cc7a41d8";
```

