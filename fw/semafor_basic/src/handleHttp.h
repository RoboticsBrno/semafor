#pragma once

#include "Arduino.h"

/** Handle root or redirect to captive portal */
void handleRoot();
void handleDataSave();
void handleAddParam();
void handleStyle();

void softApEnable();
void softApDisable();

bool isIp(String str);
String toStringIp(IPAddress ip);