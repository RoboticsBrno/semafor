#ifndef _HANDLE_HTTP_
#define _HANDLE_HTTP_

#include "Arduino.h"

/** Handle root or redirect to captive portal */
void handleRoot();
void handleDataSave();
void handleAddParam();
void handleStyle();

void conntectToWifi();
void softApEnable();
void softApDisable();

bool isIp(String str);
String toStringIp(IPAddress ip);

#endif