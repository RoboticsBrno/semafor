#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

#include "stateVector.h"
#include "EEPROM_data.h"
#include "handleHttp.h"

StateVector stateVector;
EEPROM_data stateVector_eeprom(&stateVector, sizeof(stateVector));

// DNS server
uint8_t DNS_PORT = 53;
DNSServer dnsServer;

// Web server
ESP8266WebServer server(80);

char *softAP_ssid = "semafor00";
const char *softAP_password = "adminadmin";

uint8_t button = 0;   //IO, pulled up, boot fail if low
uint8_t ledPins[] = {1, 2, 3}; //R(TX), G, B(RX)

uint32_t prevCycle = 0;
uint16_t periodCycle = 20;
uint16_t debounce = 1000;


void printInfo() {
    Serial.printf("semaforID: %d\n", stateVector.semaforID);
    Serial.printf("currentMode: %d\n", stateVector.currentMode);
    Serial.printf("monopolyDelayMin: %d\n", stateVector.monopolyDelayMin);
    Serial.printf("monopolyDelayMax: %d\n", stateVector.monopolyDelayMax);
    Serial.printf("tdPressShort: %d\n", stateVector.tdPressShort);
    Serial.printf("tdPressLong: %d\n", stateVector.tdPressLong);
}

void setup() {
    //Serial.begin(115200);
    //stateVector_eeprom.write();
    stateVector_eeprom.read();
    sprintf(softAP_ssid, "semafor%d", stateVector.semaforID);

    for(uint8_t i = 0; i < 3; ++i) {
        pinMode(ledPins[i], OUTPUT);
    }
    pinMode(button, INPUT_PULLUP);

    SPIFFS.begin();
    softApEnable();

    server.on("/", handleRoot);
    server.on("/datasave", handlaDataSave);
    server.on("/generate_204", handleRoot);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
    server.on("/fwlink", handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
    server.serveStatic("/style.css", SPIFFS, "/style.css");
    server.begin(); // Web server start

    randomSeed(analogRead(A0));
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
    if(millis() > prevCycle + periodCycle) {
        prevCycle = millis();
        //printInfo();

        switch(stateVector.currentMode) {
            case 0: //monopoly
                static uint32_t changeDelay = 0;
                static uint32_t lastChange = 0;
                static bool lightStateRG = 0; //0-red, 1-green
                if(millis() > (lastChange + changeDelay)) {
                    lastChange = millis();
                    changeDelay = 1000 * random(stateVector.monopolyDelayMin, stateVector.monopolyDelayMax);

                    lightStateRG = !lightStateRG;
                }
                digitalWrite(ledPins[0], !lightStateRG);
                digitalWrite(ledPins[1], lightStateRG);
                digitalWrite(ledPins[2], 0);
                break;
            case 1: //vabicka
                static int8_t lightState = 0;  //0-R, 1-G, 2-B, 3-nothing
                static uint32_t lastPress = 0;
                if(!digitalRead(button) && millis() > (lastPress + debounce)) {
                    lastPress = millis();
                    ++lightState;
                    if(lightState > 2)
                        lightState = 0;
                }
                for(uint8_t i = 0; i < 3; ++i) {
                    digitalWrite(ledPins[i], i == lightState);
                }
                break;
            case 2: //vlajky
                if(!digitalRead(button) && millis() > (lastPress + debounce)) {
                    lastPress = millis();
                    ++lightState;
                    if(lightState > 3)
                        lightState = 0;
                }
                for(uint8_t i = 0; i < 3; ++i) {
                    digitalWrite(ledPins[i], i == lightState);
                }
                break;
            case 3: //towerDefence
                static bool buildState = 0; //0-building, 1-destroying
                static bool prevButtonState = 0;
                static uint32_t buttonPressedSince = 0;
                if(!buildState) {
                    bool buttonState = digitalRead(button);
                    if((buttonState && !prevButtonState) && millis() > (lastPress + debounce)) {
                        lastPress = millis();
                        ++lightState;   //0-nothing, 1-R, 2-RG, 3-RGB
                        if(lightState > 3)
                            lightState = 3;
                    }
                    prevButtonState = buttonState;
                }
                else {
                    static uint32_t buttonPressedSinceShort = 0;
                    if(!digitalRead(button)) {
                        if(buttonPressedSinceShort == 0)
                            buttonPressedSinceShort = millis();
                        if(millis() > buttonPressedSinceShort + 1000*stateVector.tdPressShort) {
                            buttonPressedSinceShort = millis();
                            --lightState;
                            if(lightState < 0)
                                lightState = 0;
                        }
                    }
                    else {
                        buttonPressedSinceShort = 0;
                    }
                }

                if(!digitalRead(button)) {
                    if(buttonPressedSince == 0)
                        buttonPressedSince = millis();
                    if(millis() > buttonPressedSince + 1000*stateVector.tdPressLong) {
                        buttonPressedSince = millis();
                        buildState = !buildState;
                        lightState = buildState ? lightState + 1 : -1;
                    }
                }
                else {
                    buttonPressedSince = 0;
                }

                for(uint8_t i = 0; i < 3; ++i) {
                    digitalWrite(ledPins[i], i < lightState);
                }
                //Serial.printf("%d   %d\n", buildState, lightState);
                break;
            default:
                stateVector.currentMode = 0;
        }
    }

    /*for(uint8_t i = 0; i < 3; ++i) {
        digitalWrite(ledPins[i], 1);
        if(!digitalRead(button)) {
            for(uint8_t j = 0; j < 3; ++j) {
                digitalWrite(ledPins[j], 1);
            }
        }
        delay(200);
        digitalWrite(ledPins[i], 0);
    }*/

}