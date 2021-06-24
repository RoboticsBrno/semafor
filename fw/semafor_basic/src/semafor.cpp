#include "Arduino.h"
#include "stateVector.h"
#include "semafor.h"

#define SECOND 1000

String wifi_ssid = "Semafor";
String wifi_password = "adminadmin";
IPAddress wifiIP(192, 168, 1, 1);
IPAddress wifiIPtest(192, 168, 1, 100);
IPAddress netMsk(255, 255, 255, 0);

DNSServer dnsServer;
ESP8266WebServer server(80);

StateVector stateVector;
uint8_t semaforID = 0;
EEPROM_data stateVector_eeprom(&stateVector, sizeof(stateVector));
EEPROM_data semaforID_eeprom(&semaforID, sizeof(semaforID));

WiFiUDP udpSett;
uint8_t udpCheckKey;

uint8_t prevMode = 0;
uint8_t DNS_PORT = 53;


// bool activeLed = false;

void setLed(uint8_t ledID, bool on) {
    if(stateVector.activeLed) {
        analogWrite(ledPins[ledID], on*stateVector.ledBrightness[0]*4);
    }
}
void setLeds(bool red, bool green, bool blue) {
    setLed(0, red);
    setLed(1, green);
    setLed(2, blue);
}
void setLedsAll(bool state) {
    setLed(0, state);
    setLed(1, state);
    setLed(2, state);
}

bool buttonPressed(uint16_t timeMs, bool activeEnd = 1) {
    static uint32_t lastPressed = 0;

    if(activeEnd) {
        if(digitalRead(button)) {
            // not pressed
            lastPressed = 0;
        }
        else if(lastPressed == 0) {
            // firstly pressed
            lastPressed = millis();
        }
        else if(millis() > (lastPressed + timeMs)) {
            // still pressed after time period
            lastPressed = 0;
            return true;
        }
    }
    else {
        if(!digitalRead(button) && millis() > (lastPressed + timeMs)) {
            lastPressed = millis();
            return true;
        }
    }
    
    return false;
}

void printInfo(semState state) {
    Serial.printf("semaforID: %d\n", semaforID);
    Serial.printf("semafor state: %d\n", state);
    Serial.printf("gameMode: %d\n", stateVector.gameMode);
    Serial.printf("monopolyDelayMin: %d\n", stateVector.monopolyDelayMin);
    Serial.printf("monopolyDelayMax: %d\n", stateVector.monopolyDelayMax);
    Serial.printf("tdPressShort: %d\n", stateVector.tdPressShort);
    Serial.printf("tdPressLong: %d\n", stateVector.tdPressLong);
    Serial.printf("\n");
}

void initLeds() {
    for(uint8_t i = 0; i < 3; ++i) {
        pinMode(ledPins[i], OUTPUT);
    }
}

void initSerial() {
    Serial.begin(115200);
    for(uint8_t i = 0; i < 3; ++i) {
        pinMode(ledPins[i], SPECIAL);
    }
}


void semaforInit() {
    stateVector_eeprom.read();
    semaforID_eeprom.read();

    initLeds();

    pinMode(button, INPUT_PULLUP);

    // Serial.println("Start WiFI");
    // softApEnable();

    // server.on("/", handleRoot);
    // server.on("/datasave", handleDataSave);
    // server.on("/addparam", handleAddParam);
    //     // put "IP/addParam?id=X" into URL for setting semafor ID
    // server.onNotFound(handleRoot);
    // server.on("/style.css", handleStyle);
    // server.begin();

    // udpSett.begin(1111); 

    randomSeed(analogRead(A0));
}

void settBrodcast() {
    static bool initBrodcast = true;
    static bool blinkCenter = true;
    static ArduinoMetronome blinker(800);
    static ArduinoMetronome sender(500);

    if(initBrodcast) {
        setLeds(true, false, true);

        Serial.println("Start WiFi");
        softApEnable();

        server.on("/", handleRoot);
        server.on("/datasave", handleDataSave);
        server.on("/addparam", handleAddParam);
            // put "IP/addParam?id=X" into URL for setting semafor ID
        server.onNotFound(handleRoot);
        server.on("/style.css", handleStyle);
        server.begin();

        udpSett.begin(1111);

        initBrodcast = false;
    }

    if(blinker.loopMs()) {
        if(blinkCenter) {
            setLeds(false, true, false);
        }
        else {
            setLeds(true, false, true);
        }
        blinkCenter = !blinkCenter;            
    }

    if(sender.loopMs()) {
        struct station_info *station_list = wifi_softap_get_station_info();
        while (station_list != NULL) {
            IPAddress station_ip = ((&station_list->ip)->addr);

            Serial.println(station_ip.toString());

            udpSett.beginPacket(station_ip, 1111);
            stateVector.randomCheckNum = semaforID;
            udpSett.write((const uint8_t *) &stateVector, sizeof(stateVector));
            // udpSett.print(semaforID);
            udpSett.endPacket();            

            station_list = STAILQ_NEXT(station_list, next);
        }
        
        wifi_softap_free_station_info();
        Serial.println();         
    }




    // int packetSize = udpSett.parsePacket();
    // if (packetSize) {
    //     udpSett.read(&udpCheckKey, sizeof(udpCheckKey));

    //     udpSett.beginPacket(udpSett.remoteIP(), 1111);
    //     stateVector.randomCheckNum = udpCheckKey;
    //     // udpSett.write((const uint8_t *) &stateVector, sizeof(stateVector));
    //     udpSett.print(udpCheckKey);
    //     udpSett.endPacket();
    //     // Serial.println("Send settings - key: " + String(stateVector.gameMode));
    //     Serial.println("Send settings - key: " + String("Key:" + String(udpCheckKey) + " -> " + String(millis())));
    // }  




}

void settReceive() {
    static bool initReceive = true;
    static bool initConnected = true;
    static bool connected = false;

    static ArduinoMetronome sender(800);
    // static uint8_t checkKey = random(1, 250);

    if(initReceive) {
        // udpSett.begin(1111);
        conntectToWifi();
        initReceive = false;
    }

    if(initConnected && WiFi.status() == WL_CONNECTED) {
        initConnected = false;
        connected = true;
        udpSett.begin(1111);
        Serial.println("Connected to WiFi");
    }

    if(connected) {

        // if(sender.loopMs()) {
        //     udpSett.beginPacket(wifiIPtest, 1111);
        //     // udpSett.write(semaforID);
        //     udpSett.println(udpSett.remoteIP());
        //     udpSett.endPacket();
        //     Serial.println("Send Req");
        // }

        // Serial.println("Send local IP: " + WiFi.localIP().toString());
        
        StateVector receive;

        // int packetSize = udpSett.parsePacket();
        // if (packetSize) {

        //     Serial.print(udpSett.read());
        //     Serial.println("Recived");


        //     udpSett.beginPacket(udpSett.remoteIP(), 1111);
        //     // udpSett.write(semaforID);
        //     udpSett.println(WiFi.localIP());
        //     udpSett.endPacket();
        //     Serial.println("Send Req");   

        int packetSize = udpSett.parsePacket();
        if (packetSize) {
            // udpSett.beginPacket(udpSett.remoteIP(), udpSett.remotePort());
            // // stateVector.currentMode = millis();
            // // udpSett.write((const uint8_t *) &stateVector, sizeof(stateVector));
            // udpSett.print(millis());
            // udpSett.endPacket();
            // // Serial.println("Send settings - key: " + String(stateVector.currentMode));
            // Serial.println("Send settings - key: " + String(millis()) + udpSett.remoteIP().toString() + String(udpSett.remotePort()));
                  
            
            int len = udpSett.read((char *) &receive, sizeof(receive));
            if(len>0) {
                Serial.println("Recived settings key: " + String(receive.randomCheckNum));
            }
            else {
                Serial.println("ERR");
            }
            
        }

    }



}


semState semaforState() {
    static semState lastState = S_NORMAL;
    semState nowState = S_NORMAL;

    static bool pressedOnStart = false;
    static int startPressedTime;
    static int startTime = millis();
    int nowTime = millis();

    if(lastState == S_BRODCAST) {
        return S_BRODCAST; // all time brodcast
    }


    if(nowTime-startTime < 120 * SECOND) {
        bool buttonState = !digitalRead(button);

        if(!buttonState) {
            pressedOnStart = false;
            nowState = S_RECEIVE; // if not pressed + to 120 sec from start
        }
        else if(buttonState && pressedOnStart == false) {
            startPressedTime = millis();
            pressedOnStart = true;
        }
        else if(pressedOnStart && nowTime-startPressedTime > 2 * SECOND) {
            nowState = S_BRODCAST; //if pressed for 3 sec + to 120 sec from start
        }

    }
    else {
        nowState = S_NORMAL; // else
    }

    lastState = nowState;
    return nowState;
}


void semaforLoop() {
    switch(stateVector.gameMode) {
        case 0:
            handleMonopoly();
            break;
        case 1:
            handleVabicka();
            break;
        case 2:
            handleVlajky();
            break;
        case 3:
            handleTowerDefence();
            break;
        case 4:
            handleHoldToGet();
            break;
        default:
            stateVector.gameMode = 0;
    }
    prevMode = stateVector.gameMode;
}


void handleMonopoly() {
    static uint32_t changeDelay = 0;
    static uint32_t lastChange = 0;
    static bool lightStateRG = 1; //0-red, 1-green
    if(stateVector.gameMode != prevMode) {
        lightStateRG = 0;
        lastChange = millis();
    }
    if(millis() > (lastChange + changeDelay)) {
        lastChange = millis();
        changeDelay = 1000 * random(stateVector.monopolyDelayMin, stateVector.monopolyDelayMax);

        lightStateRG = !lightStateRG;
    }
    setLeds(!lightStateRG, lightStateRG, 0);
}
void handleVabicka() {
    static int8_t lightState = 0;  //0-R, 1-G, 2-B, 3-nothing
    if(stateVector.gameMode != prevMode)
        lightState = 0;
    if(buttonPressed(debounce, 0)) {
        ++lightState;
        if(lightState > 2)
            lightState = 0;
    }
    for(uint8_t i = 0; i < 3; ++i) {
        setLed(i, i == lightState);
    }
}
void handleVlajky() {
    static int8_t lightState = 0;  //0-R, 1-G, 2-B, 3-nothing
    if(stateVector.gameMode != prevMode)
        lightState = 0;
    if(buttonPressed(debounce, 0)) {
        ++lightState;
        if(lightState > 3)
            lightState = 0;
    }
    for(uint8_t i = 0; i < 3; ++i) {
        setLed(i, i == lightState);
    }
}
void handleTowerDefence() {
    static uint32_t lastPress = 0;
    static int8_t lightState = 0;  //0-R, 1-G, 2-B, 3-nothing
    static uint8_t buildState = 0; //0-building, 1-short flashing, 2-destroying, 3-flashing
    static bool prevButtonState = 0, buttonState = 0;
    static uint32_t buttonPressedSince = 0;
    if(stateVector.gameMode != prevMode) {
        buildState = 0;
        lightState = 0;
    }
    switch(buildState) {
        case 0:     //building
            buttonState = digitalRead(button);
            if((buttonState && !prevButtonState) && millis() > (lastPress + debounce)) {
                lastPress = millis();
                ++lightState;   //0-nothing, 1-R, 2-RG, 3-RGB
                if(lightState > 3)
                    lightState = 3;
            }
            prevButtonState = buttonState;
            break;
        case 1:     //short flashing
            static uint32_t flashStart = 0;
            static uint8_t flashesCount = 0;
            static int8_t tempLightState;
            if(flashesCount == 0)
                tempLightState = lightState;
            if(millis() > (flashStart + flashPeriod)) {
                flashStart = millis();
                ++flashesCount;
                if(lightState)
                    lightState = 0;
                else
                    lightState = 3;
                
                if(flashesCount > 20) {
                    flashesCount = 0;
                    flashStart = 0;
                    buildState = 2;
                    lightState = tempLightState;
                }
            }
            break;
        case 2:     //destroying
            static uint32_t buttonPressedSinceShort = 0;
            if(!digitalRead(button)) {
                if(buttonPressedSinceShort == 0)
                    buttonPressedSinceShort = millis();
                if(millis() > buttonPressedSinceShort + 1000*stateVector.tdPressShort) {
                    buttonPressedSinceShort = millis();
                    --lightState;
                    if(lightState <= 0) {
                        lightState = 0;
                        buildState = 3;
                    }
                }
            }
            else {
                buttonPressedSinceShort = 0;
            }
            break;
        default:    //case 3 - flashing
            if(millis() > (flashStart + flashPeriod)) {
                flashStart = millis();
                if(lightState)
                    lightState = 0;
                else
                    lightState = 3;
            }
            break;
    }

    if(!digitalRead(button)) {
        if(buttonPressedSince == 0)
            buttonPressedSince = millis();
        if(millis() > buttonPressedSince + 1000*stateVector.tdPressLong) {
            buttonPressedSince = millis();
            if(buildState == 0) {
                buildState = 1;
            }
            else {
                buildState = 0;
                lightState = -1;
            }
            //lightState = buildState ? lightState + 1 : -1;
        }
    }
    else {
        buttonPressedSince = 0;
    }

    for(uint8_t i = 0; i < 3; ++i) {
        setLed(ledPins[i], i < lightState);
    }
    //Serial.printf("%d   %d\n", buildState, lightState);
}

void handleHoldToGet() {
    ;
}