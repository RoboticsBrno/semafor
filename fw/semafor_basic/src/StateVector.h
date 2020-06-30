#ifndef _STATE_VECTOR_
#define _STATE_VECTOR_

#include <Arduino.h>

struct StateVector{
    uint8_t semaforID = 0;
    uint8_t ledBrightness[3] = {255, 255, 255};
    uint8_t currentMode = 0;
    uint16_t monopolyDelayMin = 10;
    uint16_t monopolyDelayMax = 30;
    uint8_t tdPressShort = 5;
    uint8_t tdPressLong = 10;
};
#endif /*_STATE_VETOR_*/