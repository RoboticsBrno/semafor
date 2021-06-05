#include "semafor.h"

uint32_t prevCycle = 0;
uint16_t periodCycle = 20;

void setup() {
    //Serial.begin(115200);
    //stateVector_eeprom.write();

    semaforInit();
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
    if(millis() > prevCycle + periodCycle) {
        prevCycle = millis();
        //printInfo();
        semaforLoop();
    }
}