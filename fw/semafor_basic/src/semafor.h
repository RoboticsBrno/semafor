#pragma once

#include <Arduino.h>
#include "ArduinoMetronome.hpp"
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <DNSServer.h>

#include "stateVector.h"
#include "EEPROM_data.h"
#include "handleHttp.h"

extern IPAddress wifiIP;
extern IPAddress netMsk;

const uint8_t button = 0;   //IO, pulled up, boot fail if low
const uint8_t ledPins[] = {1, 2, 3}; //R(TX), G, B(RX)

const uint16_t debounce = 1000;
const uint16_t flashPeriod = 100;

extern String wifi_ssid;
extern String wifi_password;

extern DNSServer dnsServer;
extern ESP8266WebServer server;

extern StateVector stateVector;
extern EEPROM_data stateVector_eeprom;
extern uint8_t semaforID;
extern EEPROM_data semaforID_eeprom;

extern uint8_t DNS_PORT;
extern uint8_t prevMode;

enum semState {
	S_NORMAL,
	S_BRODCAST,
	S_RECEIVE,
};

// extern bool activeLed;

void setLed(uint8_t ledGPIO, bool on);
void setLeds(bool red, bool green, bool blue);
void setLedsAll(bool state);
bool buttonPressedFor(uint16_t timeMs);
void printInfo(semState state);

void initLeds();
void initSerial();

void settReceive();
void settBrodcast();

void semaforInit();
void semaforLoop();

semState semaforState();


void handleMonopoly();
void handleVabicka();
void handleVlajky();
void handleTowerDefence();
void handleHoldToGet();