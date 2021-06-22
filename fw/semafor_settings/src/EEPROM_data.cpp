#include <EEPROM.h>
#include "EEPROM_data.h"

EEPROM_data::EEPROM_data(void * aVariablePtr, uint8_t aVariableSize){
    variablePtr = aVariablePtr;
    variableSize = aVariableSize;

    if(freeBeginId == 0){
        EEPROM.begin(EEPROM_LENGTH);
    }

    id = freeBeginId;
    freeBeginId += variableSize;
}

void EEPROM_data::write(){
    for(uint8_t i = 0; i < variableSize; ++i){
        EEPROM.put(id + i, *(((uint8_t *)variablePtr) + i));
    }
    EEPROM.commit();
}
void EEPROM_data::read(){
    for(uint8_t i = 0; i < variableSize; ++i){
        *(((uint8_t *)variablePtr) + i) = EEPROM.read(id + i);
    }
}

uint8_t EEPROM_data::freeBeginId = 0;
