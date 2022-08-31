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


class Sekcja
{
    HardwareOutput* hw;
  //  static ApiServer* api;
    uint8_t onStan=HIGH;
    uint8_t offStan=LOW;
    bool stan=false;
    bool autoSwitchActive=false;
    unsigned long timeToSwich;
    bool stateToSwich;
    unsigned long lastStateChanged=0;
    
       
    void setState(bool state);

    void init()
    {
            if(inverted){onStan=LOW; offStan=HIGH;}
            autoSwitchActive=false;
            setOff();       
    };
    public:
     uint8_t id;
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
    
   
    int sendPostRequest(String url, String requestData);
    void parseConfig(uint8_t _id, String jsonConfig);

    /* odpowiednik z boolean robi wiecej to tylko nakladka */
    void setState(uint8_t state)
    {
        autoSwitchActive=false;
        setState( state > 0 ? true : false); 
    };
    void setOn()
    {
        autoSwitchActive=false;
        setState(true);
    };
    void setOff()
    {
        autoSwitchActive=false;
        setState(false);
    };
    void setStateForTime(uint8_t state, long secondsToSwitch)
    {
        setStateForTime(state>0?true:false,secondsToSwitch);
    }
    void setStateForTime(bool state, long secondsToSwitch)
    {
        if(state) setOnForTime(secondsToSwitch);
        else setOffForTime(secondsToSwitch);
    }
    void setOnForTime(long onForSeconds)
    {
        autoSwitchActive=true;
        stateToSwich=false;
        setState(true);
        timeToSwich=onForSeconds*1000;
    };
    void setOffForTime(long offForSeconds)
    {
        autoSwitchActive=true;
        stateToSwich=true;
        setState(false);
        timeToSwich=offForSeconds*1000;
    };
    
    void loop()
    {
        if(autoSwitchActive)
        {
            if(millis()-lastStateChanged>timeToSwich)
            {
                autoSwitchActive=false;
                setState(stateToSwich);
            }
        }

    };
    
    String getStanJson()
    {
        String ret="";
        StaticJsonDocument<JSON_SIZE> doc;
        doc["id"]=id;
        doc["stan"] =stan?"1":"0";
        doc["autoSwitchActive"]=autoSwitchActive;
        if(autoSwitchActive)
        {
            doc["timeToSwitch"]= timeToSwich;
            doc["stateToSwitch"]= stateToSwich;
        }
        doc["lastStateChanged"]= lastStateChanged;
        serializeJson(doc, ret);
        return ret;
    };
    String prepareConfigJson()
    {
        String ret="";
        StaticJsonDocument<JSON_SIZE> doc;
        doc["id"]=id;
        doc["nazwa"]=nazwa;
        doc["typ"]=typ; //fizyczna, wirtualnaREST, wirtualnaMQTT
        if(typ==SEKCJA_TYP_FIZYCZNA)
        {
            doc["pin"]=pin; //pin ktorym steruje
        }
        //todo adres hardware jesli wiecej sekcji niz 8
        doc["inverted"]=inverted; // odwrocona logika na pinie
        if(typ==SEKCJA_TYP_REST)
        {
            doc["apiOnUrl"]=apiOnUrl;
            doc["apiOnJson"]=apiOnJson;
            doc["apiOffUrl"]=apiOffUrl;
            doc["apiOffJson"]=apiOffJson;
        }
        //todo mqtt
        serializeJson(doc, ret);
        return ret;
    };
};
#endif