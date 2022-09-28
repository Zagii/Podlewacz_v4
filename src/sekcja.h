#ifndef SEKCJA_H
#define SEKCJA_H

#define SEKCJA_TYP_FIZYCZNA 0
#define SEKCJA_TYP_REST 1
#define SEKCJA_TYP_MQTT 2

#define JSON_SIZE 512
#include <Arduino.h>
#include <ESP8266WiFi.h>          // Replace with WiFi.h for ESP32
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <ArduinoJson.h>
#include "hardwareOutput.h"

#define SEKCJA_ID_BRAK -1
#define SEKCJA_ID_BLAD -2

class Sekcja
{
    HardwareOutput* hw;
  //  static ApiServer* api;
    uint8_t onStan=HIGH;
    uint8_t offStan=LOW;
    bool stan=false;
    bool autoSwitchActive=false;
    unsigned long timeToSwitch;
    bool stateToSwitch;
    unsigned long lastStateChanged=0;
    
       
   
    void init();

    public:
     int id;
     String nazwa;
     int typ;     // fizyczna, wirtualnaREST, wirtualnaMQTT
     uint8_t pin; // pin ktorym steruje
     // todo adres hardware jesli wiecej sekcji niz 8
     bool inverted; // odwrocona logika na pinie
     String apiOnUrl;
     String apiOnJson;
     String apiOffUrl;
     String apiOffJson;
     // todo     String topicCommand; //topic mqtt na ktory podlewacz wysyla komende
     // todo     String topicStatus; //topic mqtt z ktorego podlewacz otrzymuje status
    Sekcja(){};
    unsigned long getLastStateChange() {return lastStateChanged;};
    bool getStan(){return stan;};
    
    void begin(HardwareOutput *h, uint8_t _id, String jsonConfig);
    void begin(HardwareOutput *h, uint8_t _id, String _nazwa,int _typ, int _pin,bool _inverted,
        String _apiOnUrl, String _apiOnJson, String _apiOffUrl, String _apiOffJson);
    
    bool copySekcja(Sekcja * source);
    /* ustawia konfiguracje sekcji*/
    int setSekcjaFromJson(String jsonConfig);
    /* zwraca konfigracje sekcji*/
    String getSekcjaJsonString();
   
   /* wysylka rozkazu post do zmiany stanu sekcji restowej */
    int sendPostRequest(String url, String requestData);
    void parseConfig(uint8_t _id, String jsonConfig);

    /* odpowiednik z boolean robi wiecej to tylko nakladka */
    void setState(uint8_t state);
    void setState(bool state);
    void setOn();
    void setOff();
    void setStateForTime(uint8_t state, long secondsToSwitch);
    void setStateForTime(bool state, long secondsToSwitch);
    void setOnForTime(long onForSeconds);
    void setOffForTime(long offForSeconds);
    
    void loop();
    
    String getStanJson();
   // String prepareConfigJson();
};
#endif