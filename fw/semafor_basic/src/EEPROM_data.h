//This is a class for storing data in EEPROM memory.
//It is intended to be used in the following way:
//  1. Create an object of this class
//  2. Initialize it using the init method
//  3. Use the write method to write data to the EEPROM
//  4. Use the read method to read data from the EEPROM
//The class supports different types of variables, and allows you to use any number of variables.

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