#ifndef CONFIG_FILE_SEKCJE_H
#define CONFIG_FILE_SEKCJE_H

//#include "apiServer.h"
#include "hardwareOutput.h"
#include "configFS.h"
#include "sekcja.h"


#define JSON_SIZE_SEKCJE 1024
#define TAG_CONFIG_FILE_SEKCJE "sekcje"

#define MAX_LICZBA_SEKCJI 16


class ConfigFileSekcje : public ConfigFS
{
    Sekcja * sekcje[MAX_LICZBA_SEKCJI];
    uint8_t liczbaSekcji=0;
    HardwareOutput *hardwareOutput;
 //   ApiServer* apiServer;
    public:
    
     
        ConfigFileSekcje(const char* file):ConfigFS(file){};
        void begin(HardwareOutput *_h/*, ApiServer *_api*/);
 
        virtual bool parseFile(String json);
        bool setSekcjeConfig(String json);
        bool setSekcjeStan(String json);

        String getSekcjeStan();

        void loop();
        
        virtual String prepareFile()
        {   
            Serial.println("configFileSekcje::prepareFile ");
            String ret="";
            StaticJsonDocument<JSON_SIZE> doc;
            doc["tag"]=TAG_CONFIG_FILE_SEKCJE;
            for(int i=0;i<liczbaSekcji;i++)
            {
                StaticJsonDocument<JSON_SIZE> doc2;
                DeserializationError error = deserializeJson(doc2, sekcje[i]->prepareConfigJson());
                if (error) {
                     Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
                     return "json error";
                }
                doc["Sekcje"].add(doc2.as<JsonObject>());
            }
            serializeJson(doc, ret);
            return ret;
        };
        

};

#endif