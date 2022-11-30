#ifndef MANAGER_H
#define MANAGER_H

#include<Arduino.h>
#include "config/config.h"
#include "czas.h"
#include "program.h"

#define TAG_STATUS "status"
#define BRAK_PROGRAMU 254

class Manager
{
    Czas* czas;
    Config* config;
    uint8_t lastRunProgramId=ID_PROGRAMU_NIEZNANE;
    int uruchomionyProgramId=BRAK_PROGRAMU;
    int uruchomionyProgramIndex=0;
    unsigned long czasOstatniegoTestu=0;
    unsigned long obecnaSekundaDzialaniaProgramu=0;
    double korekta;
    int lastTestHhMM=0;
    void sprawdzSekwencje(unsigned long obecnaSekundaDzialaniaProgramu);
    public:
        Manager(){};
        void begin(Czas* _czas, Config* _config)
        {
            czas=_czas; config=_config;
        }
        //long getTimeToNextProgramStart(){return 0;};
        int sprawdzKtoryUruchomicProgram(int d,int hhmm);
        void loop();
        bool startProgram(uint8_t programId,double _korekta, unsigned long czasStartu=0);
        bool stopProgram();
        String getStatusJson();


};

#endif