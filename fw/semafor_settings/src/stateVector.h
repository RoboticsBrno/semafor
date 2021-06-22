#pragma once

#include <Arduino.h>

struct StateVector{
    uint8_t ledBrightness[3] = {255, 255, 255};
    uint8_t currentMode = 0;
    uint16_t monopolyDelayMin = 10;
    uint16_t monopolyDelayMax = 30;
    uint8_t tdPressShort = 7;
    uint8_t tdPressLong = 12;
    uint8_t holdToGetTimeout = 10;
};