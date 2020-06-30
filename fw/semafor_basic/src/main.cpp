#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>

#include "handleHttp.h"
#include "EEPROM_data.h"
#include "stateVector.h"

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


StateVector stateVector;

void printInfo() {
    Serial.printf("semaforID: %d\n", stateVector.semaforID);
    Serial.printf("currentMode: %d\n", stateVector.currentMode);
    Serial.printf("monopolyDelayMin: %d\n", stateVector.monopolyDelayMin);
    Serial.printf("monopolyDelayMax: %d\n", stateVector.monopolyDelayMax);
    Serial.printf("tdPressShort: %d\n", stateVector.tdPressShort);
    Serial.printf("tdPressLong: %d\n", stateVector.tdPressLong);
}

EEPROM_data stateVector_eeprom(&stateVector, sizeof(stateVector));

void setup() {
    Serial.begin(115200);
    //stateVector_eeprom.write();
    stateVector_eeprom.read();
    printInfo();

    /*for(uint8_t i = 0; i < 3; ++i) {
        pinMode(ledPins[i], OUTPUT);
    }
    pinMode(button, INPUT_PULLUP);*/

    Serial.println(SPIFFS.begin() ? "SPIFFS Mount succesfull" : "SPIFFS Mount failed");
    softApEnable();

    server.on("/", handleRoot);
    server.on("/generate_204", handleRoot);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
    server.on("/fwlink", handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
    server.serveStatic("/style.css", SPIFFS, "/style.css");
    server.begin(); // Web server start
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
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