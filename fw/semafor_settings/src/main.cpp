#include "semafor.h"
#include "ArduinoMetronome.hpp"

uint16_t periodCycle = 20;
ArduinoMetronome loopMain(periodCycle);
ArduinoMetronome loopPrint(10);
ArduinoMetronome loopReceive(10);
ArduinoMetronome loopBrodcast(50);

void setup() {
    // Serial.begin(115200);

    IntiState initState = semaforInit();

    while (true)
    {
        dnsServer.processNextRequest();
        server.handleClient();

    
        switch (initState)
        {
        case SETT_RECEIVE:
            if(loopReceive.loopMs()) {
                settReceive();
            }
            break;
            //jump to semafor programm
        case NORMAL:  
            if(loopMain.loopMs()) {
                semaforLoop();
            }
            break;

        case SETT_BRODCAST:
            if(loopBrodcast.loopMs()) {
                settBrodcast();
            }
            break;
        };

        // if(loopMain.loopMs()) {
        //     semaforLoop();
        // }

        if(loopPrint.loopMs()) {
            // printInfo();
            // testUpd();
        }        
    }
}

void loop() {}