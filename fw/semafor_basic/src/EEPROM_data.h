#ifndef _EEPROM_DATA_
#define _EEPROM_DATA_

#include <EEPROM.h>

#define EEPROM_LENGTH 255   //[bytes]

class EEPROM_data{
    void * variablePtr = NULL;
    uint8_t variableSize = 0;
    uint8_t id = 0;
    static uint8_t freeBeginId;
public:
    EEPROM_data(void * aVariablePtr, uint8_t aVariableSize);
    void write(){
        for(uint8_t i = 0; i < variableSize; ++i){
            EEPROM.put(id + i, *(((uint8_t *)variablePtr) + i));
        }
        EEPROM.commit();
    }
    void read(){
        for(uint8_t i = 0; i < variableSize; ++i){
            *(((uint8_t *)variablePtr) + i) = EEPROM.read(id + i);
        }
    }
};

EEPROM_data::EEPROM_data(void * aVariablePtr, uint8_t aVariableSize){
        variablePtr = aVariablePtr;
        variableSize = aVariableSize;

        if(freeBeginId == 0){
            EEPROM.begin(EEPROM_LENGTH);
        }

        id = freeBeginId;
        freeBeginId += variableSize;
    }

uint8_t EEPROM_data::freeBeginId = 0;

#endif /*_EEPROM_DATA_*/