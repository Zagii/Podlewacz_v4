#include <Arduino.h>
#include <ESP8266WiFi.h>          // Replace with WiFi.h for ESP32
#include <ESP8266WebServer.h>     // Replace with WebServer.h for ESP32
#include <AutoConnect.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <PCF8574.h>
#include <WiFiUdp.h>
#include "defy.h"
#include "autoConnectSetup.h"
#include "oled.h"
#include "config/config.h"
#include "hardwareOutput.h"
#include "czas.h"
#include "apiServer.h"
#include "sekcja.h"
#include "manager.h"
//class ConfigAll;

Oled oled;
HardwareOutput hw;

WiFiUDP ntpUDP;
Czas czas(ntpUDP);

Config config;

ESP8266WebServer server;          // Replace with WebServer for ESP32
bool whileCPLoop(void);
AutoConnectSetup ac(server,whileCPLoop);
ApiServer apiServer;

#define LOOP_SMA_ILOSC 10
unsigned long loop_ms=0, loop_sma=0,loop_sum=0,loop_msStart=0;
uint32_t freeMemLow;

Manager manager;

void setup() {
  freeMemLow=ESP.getFreeHeap();
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Podlewacz - inicjalizacja");
  LittleFS.begin();
  hw.begin();
 
  config.begin(&czas,&hw);
 
  czas.begin(config.sysConf.useNtp,config.sysConf.ntpHost,config.sysConf.ntpOffset);
 
  //Serial.println(sysConf.prepareFile());
 
  oled.begin();
  oled.drawNetworkView();
  ac.begin();
  //ESP8266WebServer *pServer=&server;
  manager.begin(&czas,&config);
  apiServer.begin(&server, &config,&manager); 
 
}

unsigned long ms=0;
/* ta funkcja jest wywolywana w trybie offline gdy portal sie pojawia, 
równolegle wołam ja takze wewnatrz loop, gdy wifi dziala poprawnie*/

bool whileCPLoop(void) {
  bool  rc=true;
  // Here, something to process while the captive portal is open.
  // To escape from the captive portal loop, this exit function returns false.
  // rc = true;, or rc = false;
   //hw.loop();
    
    czas.loop();
    config.loop();
    if(millis()-ms>300)
    {
      ms=millis();
      oled.drawTimeView(czas.getTimeStringRTC(),czas.getTimeStringNTP(),loop_sma,freeMemLow);
    }

    manager.loop();

  return rc;
}


void loop() 
{
 loop_msStart=micros();   
  
  ac.loop();
 
  //  delay(5);
  whileCPLoop();
  // liczenie srednich przebiegow petli loop
  loop_ms=micros()-loop_msStart;
  loop_sma=loop_ms;
  //loop_sum=loop_sum - loop_sum/LOOP_SMA_ILOSC;  //odejmuje srednia wartosc od sumy
  //loop_sum+=loop_ms; // dodaje ostatni czas
  //loop_sma=loop_sum/LOOP_SMA_ILOSC; //licze czas z n przebiegów
  uint32_t freeMem=ESP.getFreeHeap();
  if(freeMem<freeMemLow) freeMemLow=freeMem;

}