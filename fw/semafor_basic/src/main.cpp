#include "semafor.h"
#include "ArduinoMetronome.hpp"



void setup() {

    uint16_t periodCycle = 20;
    ArduinoMetronome loopMain(periodCycle);
    ArduinoMetronome loopPrint(10000);
    // ArduinoMetronome loopSettings(10);

    semState sState;

    semaforInit();

    while (true)
    {
        dnsServer.processNextRequest();
        server.handleClient();


        // if(loopMain.loopMs()) {
        //     semaforLoop();
        // }

        if(loopPrint.loopMs()) {
            printInfo(sState);
        }   

        
        if(loopMain.loopMs()) {
            sState = semaforState();
            switch (sState)
            {
            case S_RECEIVE:
                Serial.printf("\rS_RECEIVE");
                settReceive();

                
                // jump to normal mode
                break;

            case S_NORMAL:
                Serial.printf("\rS_NORMAL");

                semaforLoop();
                break;
            
            case S_BRODCAST:
                Serial.printf("\rS_BRODCAST");
                settBrodcast();

                break;
            }

        }     
    }
    
}

void loop() {}