#include "semafor.h"
#include "ArduinoMetronome.hpp"



void setup() {

    uint16_t periodCycle = 20;
    ArduinoMetronome loopMain(periodCycle);
    ArduinoMetronome loopPrint(4000);
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
            // printInfo();
        }   

        
        if(loopMain.loopMs()) {
            sState = semaforState();
            switch (sState)
            {
            case S_RECEIVE:
                Serial.println("S_RECEIVE");
                settReceive();

                
                // jump to normal mode
                break;

            case S_NORMAL:
                Serial.println("S_NORMAL");

                semaforLoop();
                break;
            
            case S_BRODCAST:
                Serial.println("S_BRODCAST");
                settBrodcast();

                break;
            }

        }     
    }
    
}

void loop() {}