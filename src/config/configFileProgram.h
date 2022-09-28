#ifndef CONFIG_FILE_PROGRAM_H
#define CONFIG_FILE_PROGRAM_H

#include <LittleFS.h>
#include<ArduinoJson.h>
#include "program.h"
#include "sekwencja.h"

//#include "configFS.h"

//#define JSON_SIZE_PROGRAM 512
#define TAG_CONFIG_FILE_PROGRAMY "programy"
#define TAG_CONFIG_FILE_SEKWENCJE "sekwencje"

#define MAX_PROGRAM_SZT 16
#define MAX_SEKWENCJE_SZT 30

#define TYP_PROGRAM 0
#define TYP_SEKCJA 1


class ConfigFileProgram //: public ConfigFS
{   
      public:
        int getFirstEmptyProgramId();
        uint8_t getProgramById(uint8_t id);
        int getFirstEmptySekwencjaId();//todo do testu
        uint8_t getSekwencjaById(uint8_t id);//todo do testu
  
        Program* programyTab[MAX_PROGRAM_SZT];
        uint8_t liczbaProgramow=0;
        Sekwencja *sekwencjeTab[MAX_SEKWENCJE_SZT];
        uint8_t liczbaSekwencji=0;
        char filenameProgramy[16];
        char filenameSekwencje[16];
        ConfigFileProgram(const char* _fileProgramy, const char* _fileSekwencje)/*:ConfigFS(file)*/
        { 
            strcpy(filenameProgramy,_fileProgramy);
            strcpy(filenameSekwencje,_fileSekwencje);
        };
        ~ConfigFileProgram()
        {
                for(int i=0;i<liczbaProgramow;i++) delete programyTab[i];
                liczbaProgramow=0;
                for(int i=0;i<liczbaSekwencji;i++) delete sekwencjeTab[i];
                liczbaSekwencji=0;
        };
        int loadProgramsFromFile(); 
        int saveProgramsToFile();   
        
        void calcCzasTrwaniaProgramow();

        void begin()
        {
            loadProgramsFromFile();
            loadSekwencjeFromFile();
        };   
        void loop(){};
        
        bool addProgram(String json);   
        bool addProgramAndSaveFile(String json);   

        String getProgramyJsonString(bool dodajLastRunProgramu=false);   
        bool changeProgramFromJsonStringAndSaveFile(String json);   
        bool delProgramFromJsonString(String json);
        bool delProgram(uint8_t id);   
        
        int loadSekwencjeFromFile(); 
        int saveSekwencjeToFile(); 

        bool addSekwencja(String json);   
        bool addSekwencjaAndSaveFile(String json);      
        String getSekwencjeJsonString(bool dodajLastRunSekwencji=false);  
        bool changeSekwencjaFromJsonStringAndSaveFile(String json);   
        bool delSekwencjaFromJsonString(String json);
        bool delSekwencja(uint8_t id);     

};
#endif