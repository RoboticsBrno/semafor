#pragma once

#include <Arduino.h>

struct StateVector{
    uint8_t ledBrightness[3] = {255, 255, 255};
    uint8_t gameMode = 0;
    uint16_t monopolyDelayMin = 10;
    uint16_t monopolyDelayMax = 30;
    uint8_t tdPressShort = 7;
    uint8_t tdPressLong = 12;
    uint8_t holdToGetTimeout = 10;
    uint16_t minutkaTimeSecAll = 60*4; //min
    uint16_t mikrovlnkaTimeSecAll = 60*4; //min
    uint8_t transmittCheckNum = 111;
    bool activeLed = true;
};