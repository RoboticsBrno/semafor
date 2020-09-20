#include "handleHttp.h"
#include <ESP8266WebServer.h>
#include "tools.h"
#include <DNSServer.h>
#include <FS.h>

#include "stateVector.h"
#include "EEPROM_data.h"

extern ESP8266WebServer server;
extern char * softAP_ssid;
extern char * softAP_password;
extern DNSServer dnsServer;
extern uint8_t DNS_PORT;
extern char passwordEntry[11];
extern char deviceName[10];
extern StateVector stateVector;
extern EEPROM_data stateVector_eeprom;

extern uint8_t state;

/* Soft AP network parameters */
IPAddress apIP(192, 168, 0, 1);
IPAddress netMsk(255, 255, 255, 0);

void handleRoot() {
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");

    File f = SPIFFS.open("/task1.html", "r");
    server.streamFile(f, "text/html");
    f.close();
}
void handleTask2() {
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");

    File f = SPIFFS.open("/task2.html", "r");
    server.streamFile(f, "text/html");
    f.close();
}
void handleTask3() {
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");

    File f = SPIFFS.open("/task3.html", "r");
    server.streamFile(f, "text/html");
    f.close();
}
void handleResult() {
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");

    File f = SPIFFS.open("/result.html", "r");
    server.streamFile(f, "text/html");
    f.close();
}

void handlaDataSave() {
    Serial.println("handlaDataSave");
    char buffer[120];

    server.arg("odpoved1").toCharArray(buffer, sizeof(buffer) - 1);
    if(strcmp(buffer, "vitr") == 0 || strcmp(buffer, "Vitr") == 0 || strcmp(buffer, "vítr") == 0 || strcmp(buffer, "Vítr") == 0) {
        server.sendHeader("Location", "/task2ZcEn7", true);
        server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        server.sendHeader("Pragma", "no-cache");
        server.sendHeader("Expires", "-1");
        server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
        server.client().stop(); // Stop is needed because we sent no content length
        state = 1;
    }
    else if(strcmp(buffer, "vajicko") == 0 || strcmp(buffer, "Vajicko") == 0 || strcmp(buffer, "vajíčko") == 0 || strcmp(buffer, "Vajíčko") == 0 || strcmp(buffer, "vejce") == 0 || strcmp(buffer, "Vejce") == 0) {
        server.sendHeader("Location", "/task3emibQ", true);
        server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        server.sendHeader("Pragma", "no-cache");
        server.sendHeader("Expires", "-1");
        server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
        server.client().stop(); // Stop is needed because we sent no content length
        state = 2;
    }
    else if(strcmp(buffer, "hora") == 0 || strcmp(buffer, "Hora") == 0 || strcmp(buffer, "skala") == 0 || strcmp(buffer, "skála") == 0) {
        server.sendHeader("Location", "/resultO1mHB", true);
        server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        server.sendHeader("Pragma", "no-cache");
        server.sendHeader("Expires", "-1");
        server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
        server.client().stop(); // Stop is needed because we sent no content length
        state = 3;
    }
    else {
        server.sendHeader("Location", "/", true);
        server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        server.sendHeader("Pragma", "no-cache");
        server.sendHeader("Expires", "-1");
        server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
        server.client().stop(); // Stop is needed because we sent no content length
        state = 0;
    }


    printf("%s\n", buffer);
}

void softApEnable() {

    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(softAP_ssid);
    delay(500); // Without delay I've seen the IP address blank

    /* Setup the DNS server redirecting all the domains to the apIP */
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", apIP);
}

void softApDisable() {
    WiFi.softAPdisconnect(1);
}

void handleNotFound() {
    File f = SPIFFS.open("/notFound.html", "r");
    server.streamFile(f, "text/html");
    f.close();
}
