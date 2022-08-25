#include <Arduino.h>
#include <ESP8266WiFi.h>          // Replace with WiFi.h for ESP32
#include <ESP8266WebServer.h>     // Replace with WebServer.h for ESP32
#include <AutoConnect.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <PCF8574.h>
#include <WiFiUdp.h>
#include "autoConnectSetup.h"
#include "oled.h"
#include "hardwareOutput.h"
#include "configurationFS.h"
#include "czas.h"
#include "webServer.h"


Oled oled;
HardwareOutput hw;

WiFiUDP ntpUDP;
Czas czas(ntpUDP);

Config config;

ESP8266WebServer server;          // Replace with WebServer for ESP32
bool whileCPLoop(void);
AutoConnectSetup ac(server,whileCPLoop);
ApiServer apiServer;



void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Podlewacz - inicjalizacja");
  hw.begin();
 
  config.begin(&czas);
 
  czas.begin(config.sysConf.ntpHost,config.sysConf.ntpOffset);
  LittleFS.begin();
  //Serial.println(sysConf.prepareFile());
 
  oled.begin();
  oled.drawNetworkView();
  ac.begin();
  //ESP8266WebServer *pServer=&server;
  apiServer.begin(&server,&config); 
}

unsigned long ms=0;
/* ta funkcja jest wywolywana w trybie offline gdy portal sie pojawia, 
równolegle wołam ja takze wewnatrz loop, gdy wifi dziala poprawnie*/

bool whileCPLoop(void) {
  bool  rc=true;
  // Here, something to process while the captive portal is open.
  // To escape from the captive portal loop, this exit function returns false.
  // rc = true;, or rc = false;
   hw.loop();
    czas.loop();
    if(millis()-ms>300)
    {
      ms=millis();
      oled.drawTimeView(czas.getTimeStringRTC(),czas.getTimeStringNTP());
    }
  return rc;
}


void loop() 
{
    
    ac.loop();
  //  delay(5);
   whileCPLoop();
}