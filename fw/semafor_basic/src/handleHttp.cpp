#include "semafor.h"
#include "handleHttp.h"
#include "web_files.h"

/* Soft AP network parameters */
IPAddress apIP(192, 168, 1, 1);
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
        String(semaforID) +
        String(F(
                "</h1>"
                "Zvol parametry herních módů. Aktuálně nastavené parametry jsou zobrazeny v polích pro zadávání. Uložení nových parametrů spolu s výběrem herního módu proveď příslušným tlačítkem. Parametry a aktuálně zvolený herní mód budou uloženy a použity po restartu.<br>"
                "Aktuálně zvolený herní mód: ")) +
        String(stateVector.currentMode);

   
    Page += F(
                "<form method='POST' action='datasave'>"
                "<h2>Monopoly (0)</h2>"
                "Přeblikávání mezi červenou a zelenou v náhodném intervalu.<br>"
                "Minimální doba změny [sekundy]:<br>"
                "<input type='text' placeholder='");
    Page += String(stateVector.monopolyDelayMin);
    Page += F(
                "' name='delayMin'/><br>"
                "Maximální doba změny [sekundy]:<br>"
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
                "Doba dlouhého stisku tlačítka [seku ndy]:<br>"
                "<input type='text' placeholder='");
    Page += String(stateVector.tdPressLong);
    Page += F(            
                "' name='pressLong'/><br>"
                "<input type='submit' name='towerDefence' value='Ulož a aktivuj Tower Defence'/><br>");
    Page += F(
                "<h2>Hold to Get (4)</h2>"
                "Při stisku tlačítka zabliká a rozsvítí se červeně. Po uplynutí nastavené doby stisku zabliká a rozsvítí se zeleně. Poté opět zhasne.<br>"
                "Doba stisku tlačítka [sekundy]:<br>"
                "<input type='text' placeholder='");
    Page += String(stateVector.holdToGetTimeout);
    Page += F(            
                "' name='holdToGet_press'/><br>"
                "<input type='submit' name='holdToGet' value='Ulož a aktivuj Hold to Get'/><br>");
    Page += F(
                "<h2>Nastavení jasu LEDek</h2>"
                "Nastav hodnotu 1-255 od nejnižšího po nejvyšší jas:<br>"
                "<input type='text' placeholder='");
    Page += String(stateVector.ledBrightness[0]);
    Page += F(
                "' name='brightness'/><br>"
                "<input type='submit' name='ledserial' value='Prepinac LED/Serial'/><br>"
                "</form></body></html>");
    server.send(200, "text/html", Page);
    server.client().stop(); // Stop is needed because we sent no content length
}

void handleDataSave() {
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

    server.arg("holdToGet_press").toCharArray(buffer, sizeof(buffer) - 1);
    temp = atoi(buffer);
    if(temp > 0) {
        stateVector.holdToGetTimeout = temp;
    }

    server.arg("brightness").toCharArray(buffer, sizeof(buffer) - 1);
    temp = atoi(buffer);
    if(temp > 0) {
        stateVector.ledBrightness[0] = temp;
    }
    
    if(server.hasArg("monopoly"))
        stateVector.currentMode = 0;
    else if(server.hasArg("vabicka"))
        stateVector.currentMode = 1;
    else if(server.hasArg("vlajky"))
        stateVector.currentMode = 2;
    else if(server.hasArg("towerDefence"))
        stateVector.currentMode = 3;
    else if(server.hasArg("holdToGet"))
        stateVector.currentMode = 4;
    else if(server.hasArg("ledserial")){
        activeLed = !activeLed;
        if(activeLed) {
            initLeds();
        }
        else {
            initSerial();
        }        
    }
        

    stateVector_eeprom.write();

    server.sendHeader("Location", "/", true);
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
}

void handleAddParam() {
    char buffer[10];
    int32_t temp;

    if(server.hasArg("id")) {
        server.arg("id").toCharArray(buffer, sizeof(buffer) - 1);
        temp = atoi(buffer);
        if(temp >= 0) {
            semaforID = temp;
            semaforID_eeprom.write();
        }
    }

    server.sendHeader("Location", "/", true);
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
}

void handleStyle() {
    server.send(200, "text/css", style);
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

bool isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}
