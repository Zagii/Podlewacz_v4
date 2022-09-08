#ifndef CONFIG_FILE_SYSTEM_H
#define CONFIG_FILE_SYSTEM_H
#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "czas.h"
#include "configFS.h"

#define NTP_HOST_DEF "pool.ntp.org"
#define NTP_OFFSET_DEF 2*3600
#define MQTT_HOST_DEF "no-mqtt"
#define MQTT_PORT_DEF 1881
#define MQTT_USR_DEF "mqttUsr"
#define MQTT_PWD_DEF ""


#define JSON_SIZE_SYSTEM 512
#define TAG_CONFIG_FILE_SYSTEM "system"

class ConfigFileSystem : public ConfigFS
{
    Czas* czas;
    public:
    char ntpHost[20];
    long  ntpOffset;
    uint8_t useNtp;
    char lat[16];
    char lon[16];
    char mqttHost[20];
    long mqttPort;
    char mqttUser[8];
    char mqttPwd[16];
    uint8_t useMqtt;
     
        ConfigFileSystem(const char* file):ConfigFS(file){};
        void begin(Czas* _czas)
        {
            czas=_czas;
            parseFile(readFromFile());
        };
 
        virtual bool parseFile(String json)
        {
            Serial.println("Parsowanie pliku ");//Serial.println(filename);
            StaticJsonDocument<JSON_SIZE_SYSTEM> doc;
            DeserializationError error = deserializeJson(doc, json);
            if (error) {
                     Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
                     return false;
                }
            else
            {

                strcpy(ntpHost, doc["ntpHost"] | NTP_HOST_DEF );
                ntpOffset=doc["ntpOffset"] | NTP_OFFSET_DEF;
                useNtp=doc["useNTP"] | 1;
                strcpy(lat,doc["lat"]|"0.0");
                strcpy(lon,doc["lon"]|"0.0");
                useMqtt=doc["useMqtt"] | 0;
                strcpy(mqttHost,doc["mqttHost"] | MQTT_HOST_DEF);
                mqttPort = doc["mqttPort"] | MQTT_PORT_DEF;
                strcpy(mqttUser,doc["mqttUsr"] | MQTT_USR_DEF);
                strcpy(mqttPwd,doc["mqttPwd"] | MQTT_PWD_DEF);

            }
            return true;
        };
        virtual String prepareFile()
        {   
            String ret="";
            StaticJsonDocument<JSON_SIZE_SYSTEM> doc;
            doc["tag"]= TAG_CONFIG_FILE_SYSTEM;
            doc["ntpHost"] =ntpHost;
            doc["ntpOffset"] =  ntpOffset;
            doc["useNTP"] =  useNtp;
            doc["lat"] =  lat;
            doc["lon"] = lon;
            doc["useMqtt"] = useMqtt;
            doc["mqttHost"] = mqttHost;
            doc["mqttPort"] = mqttPort;
            doc["mqttUsr"] = mqttUser;
            doc["mqttPwd"] =  mqttPwd;
            serializeJson(doc, ret);
            return ret;
        };
        void setGeoParams(const char* _lat, const char* _lon)
        {
            Serial.print("setGeoParams");
            strcpy(lat,_lat);
            strcpy(lon,_lon);
            saveToFile();
        };
        void setMqttParams(uint8_t _useMqtt,const char* _host, long _port,const char* _user,const char* _pwd)
        {
            Serial.print("setMqttParams");
            useMqtt=_useMqtt;
            strcpy(mqttHost,_host);
            mqttPort=_port;
            strcpy(mqttUser,_user);
            strcpy(mqttPwd,_pwd);
            saveToFile();
            
        };
        void setNtpParams(uint8_t _useNtp,const char* _host, long _offset)
        {
            Serial.print("setNtpParams("); Serial.print(_useNtp);Serial.print(", ");
            Serial.print(_host);Serial.print(", "); Serial.print(_offset);Serial.println(")");
            useNtp=_useNtp;
            strcpy(ntpHost,_host);
            ntpOffset=_offset;
            saveToFile();
            czas->setNTP(useNtp==1?true:false,ntpHost,ntpOffset);
            
        };

};

#endif