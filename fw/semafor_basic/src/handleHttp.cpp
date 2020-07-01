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

/* Soft AP network parameters */
IPAddress apIP(192, 168, 0, 1);
IPAddress netMsk(255, 255, 255, 0);

void handleRoot() {
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");

    String Page;
    Page += F(
                "<!DOCTYPE html><html lang='cz'><head>"
                "<meta name='theme-color' content='#404040' />"
                "<meta charset='UTF-8'>"
                "<link rel='stylesheet' type='text/css' href='style.css'>"
                "<meta name='viewport' content='width=device-width'>"
                "<title>CaptivePortal</title></head><body>"
                "<h1>Nastavení semaforu #");
    Page +=
        String(stateVector.semaforID) +
        String(F(
                "</h1>"
                "Zvol parametry herních módů. Aktuálně nastavené parametry jsou zobrazeny v polích pro zadávání. Uložení nových parametrů spolu s výběrem herního módu proveď příslušným tlačítkem. Parametry a aktuálně zvolený herní mód budou uloženy a použity po restartu.<br>"
                "Aktuálně zvolený herní mód: ")) +
        String(stateVector.currentMode);

   
    Page += F(
                "<form method='POST' action='datasave'>"
                "<h2>Monopoly (0)</h2>"
                "Přeblikávání mezi červenou a zelenou v náhodném intervalu.<br>"
                "Minimální perioda změny [sekundy]:<br>"
                "<input type='text' placeholder='");
    Page += String(stateVector.monopolyDelayMin);
    Page += F(
                "' name='delayMin'/><br>"
                "Maximální perioda změny [sekundy]:<br>"
                "<input type='text' placeholder='");
    Page += String(stateVector.monopolyDelayMax);
    Page += F(            
                "' name='delayMax'/><br>"
                "<input type='submit' name='monopoly' value='Ulož a aktivuj monopoly'/><br>");
    
    Page += F(
                "<h2>Vábička (1)</h2>"
                "Stisknutím tlačítka se střídají barvy R, G, B.<br>"
                "<input type='submit' name='vabicka' value='Aktivuj vábičku'/><br>");

    Page += F(
                "<h2>Vlajky (2)</h2>"
                "Stisknutím tlačítka se střídají barvy R, G, B a tma.<br>"
                "<input type='submit' name='vlajky' value='Aktivuj vlajky'/><br>");

    Page += F(
                "<h2>Tower Defence (3)</h2>"
                "2 módy - stavba a rozebírání zdi. Přepínání mezi nimi dlouhým stiskem tlačítka. Při stavbě zdi mačkání tlačítka postupně rozsvěcí LEDky. Při rozebírání dlouhý stisk tlačítka postupně zhasíná LEDky. Po zhasnutí všech se semafor rozbliká.<br>"
                "Doba krátkého stisku tlačítka [sekundy]:<br>"
                "<input type='text' placeholder='");
    Page += String(stateVector.tdPressShort);
    Page += F(
                "' name='pressShort'/><br>"
                "Doba dlouhého stisku tlačítka [sekundy]:<br>"
                "<input type='text' placeholder='");
    Page += String(stateVector.tdPressLong);
    Page += F(            
                "' name='pressLong'/><br>"
                "<input type='submit' name='towerDefence' value='Ulož a aktivuj Tower Defence'/><br>");

    Page += F(
                "</form></body></html>");
    server.send(200, "text/html", Page);
    server.client().stop(); // Stop is needed because we sent no content length
}

void handlaDataSave() {
    Serial.println("handlaDataSave");
    char buffer[10];
    int32_t temp;

    server.arg("delayMin").toCharArray(buffer, sizeof(buffer) - 1);
    temp = atoi(buffer);
    if(temp > 0) {
        stateVector.monopolyDelayMin = temp;
    }

    server.arg("delayMax").toCharArray(buffer, sizeof(buffer) - 1);
    temp = atoi(buffer);
    if(temp > 0) {
        stateVector.monopolyDelayMax = temp;
    }

    server.arg("pressShort").toCharArray(buffer, sizeof(buffer) - 1);
    temp = atoi(buffer);
    if(temp > 0) {
        stateVector.tdPressShort = temp;
    }

    server.arg("pressLong").toCharArray(buffer, sizeof(buffer) - 1);
    temp = atoi(buffer);
    if(temp > 0) {
        stateVector.tdPressLong = temp;
    }
    
    if(server.hasArg("monopoly"))
        stateVector.currentMode = 0;
    else if(server.hasArg("vabicka"))
        stateVector.currentMode = 1;
    else if(server.hasArg("vlajky"))
        stateVector.currentMode = 2;
    else if(server.hasArg("towerDefence"))
        stateVector.currentMode = 3;

    stateVector_eeprom.write();

    server.sendHeader("Location", "/", true);
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
}

void softApEnable() {

    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(softAP_ssid, softAP_password);
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
