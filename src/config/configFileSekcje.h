#ifndef CONFIG_FILE_SEKCJE_H
#define CONFIG_FILE_SEKCJE_H


#include <LittleFS.h>
#include<ArduinoJson.h>
#include "hardwareOutput.h"

#include "sekcja.h"


#define JSON_SIZE_SEKCJE 1512
#define TAG_CONFIG_FILE_SEKCJE "sekcje"

#define MAX_LICZBA_SEKCJI 16


class ConfigFileSekcje
{
    Sekcja * sekcje[MAX_LICZBA_SEKCJI];
    uint8_t liczbaSekcji=0;
    HardwareOutput *hardwareOutput;

    int getSekcjaById(int id);
    int getFirstEmptySekcjaId();
    char filename[16];
    public:

     
        ConfigFileSekcje(const char* _file)
        {
            strcpy(filename,_file);
        };
        void begin(HardwareOutput *_h);
 
    //    virtual bool parseFile(String json);
        
        /* ustawia konfiguracje wielu sekcji jednoczesnie */
      //  bool setSekcjeConfig(String json);
      
        /* ustawia kofiguracje jednej sekcji */
        bool setSekcjaConfig(String json);

        /* ustawia stan wielu sekcji jednocześnie */
        bool setSekcjeStan(String json);

        void setOffSekcjeAll();
        
        bool setSekcjaStan(String json);
        bool setSekcjaStan(uint8_t sekcjaId, bool stan, long secondsToSwitch);
        
        String getSekcjeStan();

        void loop();
        
   /*     virtual String prepareFile()
        {   
            Serial.println("configFileSekcje::prepareFile ");
            String ret="";
            StaticJsonDocument<JSON_SIZE_SEKCJE> doc;
            doc["tag"]=TAG_CONFIG_FILE_SEKCJE;
            for(int i=0;i<liczbaSekcji;i++)
            {
              doc["Sekcje"].add(serialized(sekcje[i]->prepareConfigJson()));
            }
            serializeJson(doc, ret);
            return ret;
        };*/
        
        int loadSekcjeFromFile(); 
        int saveSekcjeToFile(); 

        bool addChangeSekcja(String json);   
        bool addChangeSekcjaAndSaveFile(String json);      
        String getSekcjeJsonString();   
       // bool changeSekcjaFromJsonStringAndSaveFile(String json);   
        bool delSekcjaFromJsonString(String json);
        bool delSekcja(uint8_t id);     
};

#endif