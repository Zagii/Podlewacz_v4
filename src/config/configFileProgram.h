#ifndef CONFIG_FILE_PROGRAM_H
#define CONFIG_FILE_PROGRAM_H

//#include <ListLib.h>
#include<ArduinoJson.h>
#include "program.h"
#include "configFS.h"

#define JSON_SIZE_PROGRAM 512
#define TAG_CONFIG_FILE_PROGRAMY "programy"

#define MAX_PROGRAM_SZT 16

class ConfigFileProgram //: public ConfigFS
{   
        int getFirstEmptyProgramId();
        uint8_t getProgramById(uint8_t id);
    public:
        Program* programyTab[MAX_PROGRAM_SZT];
        uint8_t liczbaProgramow=0;
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
        };
        int loadProgramsFromFile(); //todo do testu
        int saveProgramsToFile();   //todo do testu
     //   virtual bool parseFile(String json);
       // virtual String prepareFile();
        void begin(){loadProgramsFromFile();};   //todo do testu
        void loop(){};
        //todo
        bool addProgram(String json);   //todo do testu
        bool addProgramAndSaveFile(String json);   //todo do testu

        String getProgramyJsonString(bool dodajLastRunProgramu=false);   //todo do testu
        //getProgramById(uint8_t id);
        bool changeProgramFromJsonString(String json);   //todo do testu
        bool delProgramFromJsonString(String json);
        bool delProgram(uint8_t id);   //todo do testu
        //addProgramSekwencja
        //changeProgramSekwencja
        //delProgramSekwencja
        
       // void addSekwencje(String json);
       // String getProgramString();

};
#endif