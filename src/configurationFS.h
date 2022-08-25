#ifndef CONFIGURATION_FS_H
#define CONFIGURATION_FS_H

#include "czas.h"

#define NTP_HOST_DEF "pool.ntp.org"
#define NTP_OFFSET_DEF 2*3600
#define MQTT_HOST_DEF "no-mqtt"
#define MQTT_PORT_DEF 1881
#define MQTT_USR_DEF "mqttUsr"
#define MQTT_PWD_DEF ""

class ConfigFS
{
        char filename[16];
     //   bool isConfigChanged = false;
       // char path[20];
    public:
        ConfigFS(const char* file)
        {
            strcpy(filename,file);
           // strcpy(path,pathToFile);
        };
        void begin()
        {
            //check if file exist
            // N: create empty file
            // Y: skip, do nothing
        }
        String readFromFile()
        {
            String ret="";
            Serial.print("config readFromFile: ");Serial.println(filename);
           // Dir d = LittleFS.openDir(path);
        // Open uploaded parameter and parse parameters with JSON
            File file =LittleFS.open(filename, "r");
            String p="{}";
            if (file) 
            {
                p="";
                while(file.available())
                {
                 p+=file.read();
                }
                file.close();
                
            }
            else
            {
                Serial.println( String(filename) + String(" open error"));
            }
            Serial.print("File data: ");Serial.println(p);
            return ret;
        };
        String loadFromFile()
        {
            String p= readFromFile();
            if( parseFile(p) )
            {
                return p;
            }
            return String("parse failed");
        };
        bool saveToFile()
        {
            if(checkDifference())
            {
                Serial.print("Saving config: "); Serial.println(filename);
                String s= prepareFile();
                Serial.println(s);
                File f=LittleFS.open(filename,"w");
                f.println(s.c_str());
                f.close();
            }
            return true;
        };

       bool checkDifference()
        {
            Serial.print("compare config file: "); Serial.print(filename);
           String s1= readFromFile();
           String s2= prepareFile();
           if( s1.compareTo(s2)==0)
           {
            Serial.println(".. not changed");
           }else
           {
            Serial.println(".. changed");
           }
            return false;
        }
        virtual bool parseFile(String json)=0;
        virtual String prepareFile()=0;

};

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
     
        ConfigFileSystem(const char* file):ConfigFS(file)
        {
            parseFile("{}");
        };
        void begin(Czas* _czas){czas=_czas;};
 
        virtual bool parseFile(String json)
        {
            StaticJsonDocument<128> doc;
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
            StaticJsonDocument<256> doc;
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
            Serial.println("setNtpParams");
            useNtp=_useNtp;
            strcpy(ntpHost,_host);
            ntpOffset=_offset;
            saveToFile();
            czas->setNTP(ntpHost,ntpOffset);
        };

};

class Config
{
    Czas* czas;
    public:
      ConfigFileSystem sysConf{"/sys.json"};
      void begin(Czas* _czas)
      {
        czas=_czas;
        sysConf.begin(czas);
      };
};
#endif