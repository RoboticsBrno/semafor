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

char *softAP_ssid = "Hadanka_Vedatori";
const char *softAP_password = "adminadmin";

uint8_t button = 0;   //IO, pulled up, boot fail if low
uint8_t ledPins[] = {1, 2, 3}; //R(TX), G, B(RX)

uint32_t prevCycle = 0;
uint16_t periodCycle = 20;
uint16_t debounce = 1000;
uint16_t flashPeriod = 100;
uint8_t state = 0;
uint32_t ledStateBegin = 0;


void setLed(uint8_t ledGPIO, bool on) {
    analogWrite(ledGPIO, on*stateVector.ledBrightness[0]*4);
}

void setup() {
    Serial.begin(115200);

    for(uint8_t i = 0; i < 3; ++i) {
        pinMode(ledPins[i], OUTPUT);
    }
    pinMode(button, INPUT_PULLUP);

    SPIFFS.begin();
    softApEnable();

    server.on("/", handleRoot);
    server.on("/task2ZcEn7", handleTask2);
    server.on("/task3emibQ", handleTask3);
    server.on("/resultO1mHB", handleResult);
    server.on("/datasave", handlaDataSave);
    server.on("/generate_204", handleRoot);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
    server.on("/fwlink", handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
    server.onNotFound(handleRoot);
    server.serveStatic("/style.css", SPIFFS, "/style.css");
    server.begin(); // Web server start
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
    if(millis() > prevCycle + periodCycle) {
        prevCycle = millis();
        /*for(uint8_t i = 0; i < state; ++i) {
            digitalWrite(ledPins[i], 1);
        }*/
        /*if((ledStateBegin == 0) && state) {
            ledStateBegin = millis();
        }
        if(state && ((millis() - ledStateBegin) > 2000)) {
            state = 0;
            ledStateBegin = 0;
        }
        printf("%d\n", state);*/
    }
}