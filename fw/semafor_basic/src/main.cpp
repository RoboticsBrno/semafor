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

const char *softAP_ssid = "semafor0";
const char *softAP_password = "adminadmin";

const char deviceName[] = "semafor0";
uint8_t button = 0;   //IO, pulled up, boot fail if low
uint8_t ledPins[] = {1, 2, 3}; //R(TX), G, B(RX)

uint32_t prevCycle = 0;
uint16_t periodCycle = 20;


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
                static bool lightState = 0; //0-red, 1-green
                if(millis() > (lastChange + changeDelay)) {
                    lastChange = millis();
                    changeDelay = 1000 * random(stateVector.monopolyDelayMin, stateVector.monopolyDelayMax);

                    lightState = !lightState;
                    digitalWrite(ledPins[0], !lightState);
                    digitalWrite(ledPins[1], lightState);
                    digitalWrite(ledPins[2], 0);
                }
                break;
            case 1: //vabicka

                break;
            case 2: //vlajky

                break;
            case 3: //towerDefence

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