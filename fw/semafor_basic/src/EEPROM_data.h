#pragma once

#define EEPROM_LENGTH 255   //[bytes]

class EEPROM_data{
    void * variablePtr = NULL;
    uint8_t variableSize = 0;
    uint8_t id = 0;
    static uint8_t freeBeginId;
public:
    EEPROM_data(void * aVariablePtr, uint8_t aVariableSize);
    void write();
    void read();
};