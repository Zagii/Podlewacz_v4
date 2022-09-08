#ifndef AUTOCONNECT_SETUP_H
#define AUTOCONNECT_SETUP_H
#include <AutoConnect.h>
#include "oled.h"
extern Oled oled;

void OTAStart() {
        Serial.println("Start OTA updating");
        oled.drawText("Start aktualizacji..",2);
        
    };

    void OTAEnd() {
        Serial.println("\nEnd");
         oled.drawText("Koniec\nwgrywania\nfirmware,\nsukces",2);
         delay(1500);
       
    };
    void OTAProgress(unsigned int amount, unsigned int size) {
        Serial.printf("Progress: %u(%u)\r", amount, size);
        double proc= (double)(amount) / (double)ESP.getFlashChipSize()*100;
        String s=String("Wgrywanie \nfirmware,\npostep: \n")+ String(proc,0)+String("%");
        oled.drawText(s,2); 
    };

    void OTAError(uint8_t error) {
        Serial.printf("Error[%u]: ", error);
        String s=String("Blad aktualizacji: ")+ String(error);
        oled.drawText(s,2);
    };
    void onConnect(IPAddress& ipaddr) {
        Serial.print("WiFi connected with ");
        Serial.print(WiFi.SSID());
        Serial.print(", IP:");
        Serial.println(ipaddr.toString());
    }
class AutoConnectSetup{
  
    AutoConnect   portal;
   
    const char* fw_ver = "0.2";
    const char* name="Podlewacz";
    char uniqName[20]="";
    AutoConnect::WhileCaptivePortalExit_ft whileCP;
    public:
   //  AutoConnectSetup(AutoConnect *p){
    AutoConnectSetup(ESP8266WebServer &s,AutoConnect::WhileCaptivePortalExit_ft fn):portal(s){
       //  portal=p;
       whileCP=fn;
        //sprintf(name,"Podlewacz-%d",ESP.getChipId());
        sprintf(uniqName,"%s-%s",name,String(ESP.getChipId(), HEX).c_str());
     };  
     //void begin(WebServerClass& webServer)
     AutoConnect * getAutoConnect()
     {
        return &portal;
     }
     void begin()
     {
       
        AutoConnectConfig config;
        portal.whileCaptivePortal(whileCP);
        config.autoReconnect = true;
       // config.retainPortal = true;
        //config.menuItems = AC_MENUITEM_OPENSSIDS | AC_MENUITEM_RESET | AC_MENUITEM_HOME;
        config.hostName=name;
        Serial.print("HostName: ");Serial.println(name);
        config.title=uniqName;
        config.apid = uniqName;
        //config.psk  = "12345678";
        /*The AutoConnect ticker indicates the WiFi connection status in the following three flicker patterns:
             Short blink: The ESP module stays in AP_STA mode.
             Short-on and long-off: No STA connection state. (i.e. WiFi.status != WL_CONNECTED)
             No blink: WiFi connection with access point established and data link enabled. (i.e. WiFi.status = WL_CONNECTED)
        */
        config.ticker = true;
        config.tickerPort = LED_BUILTIN;
        config.tickerOn = LOW;
        config.ota = AC_OTA_BUILTIN;
        config.otaExtraCaption = fw_ver;
        //portal= new AutoConnect(webServer);
        portal.onOTAStart(OTAStart);
        portal.onOTAEnd(OTAEnd);
        portal.onOTAProgress(OTAProgress);
        portal.onOTAError(OTAError);
       
        portal.onConnect(onConnect);  // Register the ConnectExit function
        portal.config(config);
        Serial.println(F("AC portal config"));
        if (portal.begin()) {
            Serial.println("WiFi connected: " + WiFi.localIP().toString());
        }
     };
     void loop()
     {
        portal.handleClient();
     };
     
};

#endif