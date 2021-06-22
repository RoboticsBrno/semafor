#include "Arduino.h"
#include "stateVector.h"
#include "semafor.h"

#define ledDisable
#define serialEnable

String wifi_ssid = "Semafor";
String wifi_password = "adminadmin";

DNSServer dnsServer;
ESP8266WebServer server(80);
IPAddress udpIP(192, 168, 1, 1);

StateVector stateVector;
uint8_t semaforID = 0;
EEPROM_data stateVector_eeprom(&stateVector, sizeof(stateVector));
EEPROM_data semaforID_eeprom(&semaforID, sizeof(semaforID));

WiFiUDP udpSett;
WiFiUDP udpSerial;

uint8_t prevMode = 0;
uint8_t DNS_PORT = 53;

char packet[255];
char reply[] = "Packet received!";

void setLed(uint8_t ledID, bool on) {
    #ifndef ledDisable
    analogWrite(ledPins[ledID], on*stateVector.ledBrightness[0]*4); // ? jak se podle toho nastavuje jas?
    #endif
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

void printInfo() {
    Serial.printf("semaforID: %d\n", semaforID);
    Serial.printf("currentMode: %d\n", stateVector.currentMode);
    Serial.printf("monopolyDelayMin: %d\n", stateVector.monopolyDelayMin);
    Serial.printf("monopolyDelayMax: %d\n", stateVector.monopolyDelayMax);
    Serial.printf("tdPressShort: %d\n", stateVector.tdPressShort);
    Serial.printf("tdPressLong: %d\n", stateVector.tdPressLong);
}

IntiState semaforInit() {
    IntiState out;

    stateVector_eeprom.read();
    semaforID_eeprom.read();

    #ifndef ledDisable
    for(uint8_t i = 0; i < 3; ++i) {
        pinMode(ledPins[i], OUTPUT);
    }
    #endif

    #ifdef serialEnable
        Serial.begin(115200);
    #endif

    pinMode(button, INPUT_PULLUP);
    delay(50);


    // while (digitalRead(button))
    // {
    //     // wait for button push
    // }

    bool wifiConnected = false;
    conntectToWifi();

    for(int count = 0; count < 20; count ++) {
        if(WiFi.status() == WL_CONNECTED) {
            wifiConnected = true;
            Serial.println("Connected");
            break;    
        }
        delay(200); // max 4 seconds
    }

    if(wifiConnected) {
        // settings receive mode
        Serial.println("Receive");
        out = SETT_RECEIVE;
    }
    else {
        // settings brodcast mode
        // no wifi with name Semafor -> start AP + settings bc server
        Serial.println("Brodcast");
        settBrodcastInit();
        out = SETT_BRODCAST;
    }

    
    randomSeed(analogRead(A0));
    return out;
}

void settReceive() {
    udpSett.beginPacket(udpIP, 1111);
    udpSett.println(WiFi.localIP());
    udpSett.endPacket();

    Serial.println("Send local IP: " + WiFi.localIP().toString());
    
    int packetSize = udpSett.parsePacket();
    if (packetSize) {
        udpSett.beginPacket(udpSett.remoteIP(), 1111);
        stateVector.currentMode = udpSett.remoteIP().toString()[-1];
        udpSett.write((const uint8_t *) &stateVector, sizeof(stateVector));
        udpSett.endPacket();
        Serial.println("Send settings - key: " + String(stateVector.currentMode));
    }

}

void settBrodcastInit() {
    setLeds(true, false, true);

    softApEnable();

    server.on("/", handleRoot);
    server.on("/datasave", handleDataSave);
    server.on("/addparam", handleAddParam);
        // put "IP/addParam?id=X" into URL for setting semafor ID
    server.onNotFound(handleRoot);
    server.on("/style.css", handleStyle);
    server.begin();

    udpSett.begin(1111);
}

void settBrodcast() {
    int packetSize = udpSett.parsePacket();
    if (packetSize) {

        int len = udpSett.read((char *) &stateVector, sizeof(stateVector));
        if(len>0) {
            Serial.println("Recived settings: " + String(stateVector.currentMode));
        }
        else {
            Serial.println("ERR");
        }
        
    }
     
}

void testUpd() {
    // udpSett.beginPacket(udpIP, 1111);
    // String Packet = "TestPacketESPtoESP";
    // const char *PacketToSend = Packet.c_str();
    // udpSett.write(PacketToSend);
    // udpSett.endPacket();
    // Serial.println(millis());



    int packetSize = udpSett.parsePacket();
    if (packetSize) {
        Serial.print("Received packet! Size: ");
        Serial.println(packetSize); 
        int len = udpSett.read(packet, 255);
        if (len > 0)
        {
        packet[len] = '\0';
        }
        Serial.print("Packet received: ");
        Serial.println(packet);
    
        // Send return packet
        udpSett.beginPacket(udpIP, 1111);
        udpSett.println(reply);
        udpSett.println(sizeof(stateVector));
        // (stateVector);
    
            // udpSett.write(i, *(((uint8_t *)&stateVector) + i));
        // Serial.write((const char *) &stateVector);
        udpSett.write((const uint8_t *) &stateVector, sizeof(stateVector));

        udpSett.endPacket();

        Serial.print(udpSett.remoteIP());
        Serial.print(" - ");
        Serial.println(udpSett.remotePort());
    
    }    
}

void semaforLoop() {
    switch(stateVector.currentMode) {
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
            stateVector.currentMode = 0;
    }
    prevMode = stateVector.currentMode;
}

void handleMonopoly() {
    static uint32_t changeDelay = 0;
    static uint32_t lastChange = 0;
    static bool lightStateRG = 1; //0-red, 1-green
    if(stateVector.currentMode != prevMode) {
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
    if(stateVector.currentMode != prevMode)
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
    if(stateVector.currentMode != prevMode)
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
    if(stateVector.currentMode != prevMode) {
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