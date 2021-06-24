#include "semafor.h"
#include "ArduinoMetronome.hpp"



void setup() {

    uint16_t periodCycle = 20;
    ArduinoMetronome loopMain(periodCycle);
    ArduinoMetronome loopPrint(10000);

    semState sState;

    semaforInit();

    sState = S_RECEIVE; // first start mode

    while (true)
    {
        dnsServer.processNextRequest();
        server.handleClient();

        if(loopPrint.loopMs()) {
            printInfo(sState);
        }   
 
        if(loopMain.loopMs()) {
            
            switch (sState)
            {
            case S_RECEIVE:
                Serial.printf("\rS_RECEIVE");
                sState = semaforState();

                if(settReceive()) {
                    sState = S_NORMAL; // jump to normal mode after new settings receive
                }
                
                semaforLoop();
                break;

            case S_NORMAL:
                Serial.printf("\rS_NORMAL");
                semaforLoop();
                break;
            
            case S_BRODCAST:
                // restart ESP to go out from this mode
                Serial.printf("\rS_BRODCAST");
                settBrodcast();
                break;
            }
        }     
    }  
}

void loop() {}