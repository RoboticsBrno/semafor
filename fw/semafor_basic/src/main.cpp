#include "semafor.h"
#include "ArduinoMetronome.hpp"



void setup() {

    uint16_t periodCycle = 20;
    ArduinoMetronome loopMain(periodCycle);
    ArduinoMetronome loopPrint(10000);
    ArduinoMetronome loopSerialRead(50);

    semState sState;

    semaforInit();


    // while (digitalRead(button))
    // {
    //     setLedsAll(1);
    //     delay(50);
    // }

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
                // Start game and wait for settings (this mode is automaticly deactivated after settings receive or 2 minutes)

                Serial.printf("\rS_RECEIVE");
                sState = semaforState(); //

                if(settReceive()) { // connec to mother`s ESP and receive settings
                    sState = S_NORMAL; // jump to normal mode after new settings receive
                }

                semaforLoop(); // run normal games parallel with settings receive
                break;

            case S_NORMAL:
                // Normal game mode - WiFi is off and game is running
                Serial.printf("\rS_NORMAL");
                semaforLoop();
                break;

            case S_BRODCAST:
                // Broadcast mode - WiFi is on and game is not running
                // ESP is in mother`s mode with enabled AP and broadcast settings to all ESPs
                // To exit from this mode unplugged power and restart ESP
                Serial.printf("\rS_BRODCAST");
                settBrodcast();
                break;
            }
        }
    }
}

void loop() {}