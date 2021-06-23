#include "semafor.h"
#include "ArduinoMetronome.hpp"

uint16_t periodCycle = 20;
ArduinoMetronome loopMain(periodCycle);

void setup() {
    //Serial.begin(115200);
    //stateVector_eeprom.write();

    semaforInit();

    while (true)
    {
        dnsServer.processNextRequest();
        server.handleClient();


        if(loopMain.loopMs()) {
            semaforLoop();
            printInfo();
        }        
    }
    
}

void loop() {}