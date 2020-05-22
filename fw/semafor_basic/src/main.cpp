#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

#include "credentials.h"

const char deviceName[] = "semafor0";
uint8_t pinTX = 1;  //IO
uint8_t pinRX = 3;  //IO
uint8_t pinLED = 2;     //IO, pulled up, boot fail if low
uint8_t pinFlash = 0;   //IO, pulled up, boot fail if low
uint8_t pins[] = {1, 2, 3};

void setup() {
    WiFi.begin(wifi_SSID, wifi_psd);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    WiFi.hostname(deviceName);
    ArduinoOTA.setHostname(deviceName);
    ArduinoOTA.begin();

    for(uint8_t i = 0; i < 3; ++i) {
        pinMode(pins[i], OUTPUT);
    }
    pinMode(pinFlash, INPUT_PULLUP);
}

void loop() {
    ArduinoOTA.handle();
    for(uint8_t i = 0; i < 3; ++i) {
        digitalWrite(pins[i], 1);
        if(!digitalRead(pinFlash)) {
            for(uint8_t j = 0; j < 3; ++j) {
                digitalWrite(pins[j], 1);
            }
        }
        delay(200);
        digitalWrite(pins[i], 0);
    }

}