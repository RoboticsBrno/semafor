#include "semafor.h"
#include "handleHttp.h"
#include "web_files.h"

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
                "<title>Semafor manager</title></head><body>"
                "<h1>Nastavení semaforu #");
    Page +=
        String(semaforID) +
        String(F(
                "</h1>"
                "Zvol parametry herních módů. Aktuálně nastavené parametry jsou zobrazeny v polích pro zadávání. Uložení nových parametrů spolu s výběrem herního módu proveď příslušným tlačítkem. Parametry a aktuálně zvolený herní mód budou uloženy a použity i po restartu.<br>"
                "Aktuálně zvolený herní mód: ")) +
        String(stateVector.gameMode);

   
    Page += F(
                "<form method='POST' action='datasave'>");

    // Monopoly (0)
    Page += F( 
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
    
    // Vábička (1)
    Page += F(
                "<h2>Vábička (1)</h2>"
                "Stisknutím tlačítka se střídají barvy R, G, B.<br>"
                "<input type='submit' name='vabicka' value='Aktivuj vábičku'/><br>");

    // Vlajky (2)
    Page += F(
                "<h2>Vlajky (2)</h2>"
                "Stisknutím tlačítka se střídají barvy R, G, B a tma.<br>"
                "<input type='submit' name='vlajky' value='Aktivuj vlajky'/><br>");

    // Tower Defence (3)
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

    // Hold to Get (4)
    Page += F(
                "<h2>Hold to Get (4)</h2>"
                "Jednoho krásného dne to někdo doprogramuje :-)");
    //             "Při stisku tlačítka zabliká a rozsvítí se červeně. Po uplynutí nastavené doby stisku zabliká a rozsvítí se zeleně. Poté opět zhasne.<br>"
    //             "Doba stisku tlačítka [sekundy]:<br>"
    //             "<input type='text' placeholder='");
    // Page += String(stateVector.holdToGetTimeout);
    // Page += F(            
    //             "' name='holdToGet_press'/><br>"
    //             "<input type='submit' name='holdToGet' value='Ulož a aktivuj Hold to Get'/><br>");


    // Minutka (5)
    Page += F( 
                "<h2>Minutka (5)</h2>"
                "Semafor zhasne všechny LEDky a čeká na zmáčknutí tlačítka, poté se celý rozsvítí.<br>"
                "Po uplynutí 1/3 času zhasne modrou LEDku, po 2/3 zhasne i zelenou LEDku.<br>"
                "10 vteřin před koncem začne blikat. Konec je signalizován zhasnutím všech LEDek.<br>"
                "Celkový čas [minuty]:<br>"
                "<input type='text' placeholder='");
    Page += String(stateVector.minutkaTimeSecAll/60);
    Page += F(
                "' name='minutkaTimeMinAll'/><br>"
                "<input type='submit' name='minutka' value='Ulož a aktivuj minutku'/><br>");

    // Mikrovlnka (5)
    Page += F( 
                "<h2>Mikrovlnka (6)</h2>"
                "Semafor rozsvítí všechny LEDky a čeká na zmáčknutí tlačítka, poté se celý zhasne.<br>"
                "Po uplynutí 1/3 času rozsvítí modrou LEDku, po 2/3 rozsvítí i zelenou LEDku.<br>"
                "10 vteřin před koncem začne blikat. Konec je signalizován rozsvícením všech LEDek.<br>"
                "Celkový čas [minuty]:<br>"
                "<input type='text' placeholder='");
    Page += String(stateVector.mikrovlnkaTimeSecAll/60);
    Page += F(
                "' name='mikrovlnkaTimeMinAll'/><br>"
                "<input type='submit' name='mikrovlnka' value='Ulož a aktivuj mikrovlnku'/><br>");                

    // Nastavení jasu LEDek
    Page += F(
                "<h2>Nastavení jasu LEDek</h2>"
                "Nastav hodnotu 1-255 od nejnižšího po nejvyšší jas:<br>"
                "<input type='text' placeholder='");
    Page += String(stateVector.ledBrightness[0]);
    Page += F(
                "' name='brightness'/><br>"
                "<input type='submit' name='led' value='Ulož jas LEDek'/><br>"
                "<br>");
    
    // End form and page
    Page += F("</form></body></html>");

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

    server.arg("minutkaTimeMinAll").toCharArray(buffer, sizeof(buffer) - 1);
    temp = atoi(buffer);
    if(temp > 0) {
        stateVector.minutkaTimeSecAll = temp*60;
    }

    server.arg("mikrovlnkaTimeMinAll").toCharArray(buffer, sizeof(buffer) - 1);
    temp = atoi(buffer);
    if(temp > 0) {
        stateVector.mikrovlnkaTimeSecAll = temp*60;
    }

    server.arg("brightness").toCharArray(buffer, sizeof(buffer) - 1);
    temp = atoi(buffer);
    if(temp > 0) {
        stateVector.ledBrightness[0] = temp;
    }
    
    if(server.hasArg("monopoly"))
        stateVector.gameMode = 0;
    else if(server.hasArg("vabicka"))
        stateVector.gameMode = 1;
    else if(server.hasArg("vlajky"))
        stateVector.gameMode = 2;
    else if(server.hasArg("towerDefence"))
        stateVector.gameMode = 3;
    else if(server.hasArg("holdToGet"))
        stateVector.gameMode = 4;
    else if(server.hasArg("minutka"))
        stateVector.gameMode = 5;
    else if(server.hasArg("mikrovlnka"))
        stateVector.gameMode = 6;

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

    if(server.hasArg("id")) {
        server.arg("id").toCharArray(buffer, sizeof(buffer) - 1);
        int sid = atoi(buffer);
        if(sid >= 0) {
            semaforID = sid;
            semaforID_eeprom.write();
        }
    }

    //1 = neable led / 0 = enable serial
    if(server.hasArg("led")) {
        server.arg("led").toCharArray(buffer, sizeof(buffer) - 1);
        int led = atoi(buffer);
        if(led == 1) {
            initLeds();
        }
        else {
            initSerial();
        }
    } 

    server.sendHeader("Location", "/", true);
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
}

void handleAdmin() {
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
                "<title>ADMIN! Semafor manager</title></head><body>"
                "<h1>ADMIN Nastavení semaforu #");
    Page +=     String(semaforID);
   
    Page += F(
                "</h1>"
                "<form method='POST' action='adminsave'>"
                "<h2>Nastavení ID</h2>"
                "ID senzoru [číslo > 0]:<br>"
                "<input type='text' placeholder='");
    Page += String(semaforID);
    Page += F(
                "' name='senID'/><br>"
                "<input type='submit' value='Ulož nové ID'/><br>");
    Page += F(
                "<h2>Prepinac LED/Serial</h2>"
                "Ledky jsou ativni: ");
    Page += String(stateVector.activeLed);
    Page += F(
                "<br>"
                "<input type='submit' name='ledserial' value='Prepinac LED/Serial'/><br>"
                "</form></body></html>");


    server.send(200, "text/html", Page);
    server.client().stop(); // Stop is needed because we sent no content length    
}

void handleAdminSave() {
    Serial.println("handleAdminSave");
    char buffer[10];
    int32_t temp;


    server.arg("senID").toCharArray(buffer, sizeof(buffer) - 1);
    temp = atoi(buffer);
    if(temp > 0) {
        semaforID = temp;
    }

    if(server.hasArg("ledserial")){
        if(stateVector.activeLed == true) {
            initSerial();
            stateVector.activeLed = false;
        }
        else {
            initLeds();
            stateVector.activeLed = true;
        }      
    } 

    stateVector_eeprom.write();

    server.sendHeader("Location", "/admin", true);
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

void wifiEnableConnect() {
    WiFi.begin(wifi_ssid, wifi_password);
}

void wifiDisable() {
    WiFi.disconnect(true);
}

void softApEnable() {
    wifiDisable();
    WiFi.softAPConfig(wifiIP, wifiIP, netMsk);
    WiFi.softAP(wifi_ssid, wifi_password);
    delay(500); // Without delay I've seen the IP address blank

    /* Setup the DNS server redirecting all the domains to the wifiIP */
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", wifiIP);
}

void softApDisable() {
    WiFi.softAPdisconnect(true);
}