#ifndef SEKWENCJA_H
#define SEKWENCJA_H
#include <Arduino.h>
#include <ArduinoJson.h>

#define JSON_SIZE_SEKWENCJA 256
#define ID_SEKWENCJI_NIEZNANE 254
#define ID_PROGRAMU_NIEZNANE 254
/* sekwencja programu inaczej pojedynczy krok*/
class Sekwencja
{
        uint8_t sekwencjaId=ID_SEKWENCJI_NIEZNANE;
        uint8_t sekcjaId;
        uint8_t programId=ID_PROGRAMU_NIEZNANE;
        bool akcja;
        unsigned long czasTrwaniaAkcji; // gdy czas==0 akcja nie powoduje antyakcji
        unsigned long startAkcji; //start akcji 
        unsigned long sekwencjaLastRun=0;
    public:
        Sekwencja(){};
        void setSekwencja(uint8_t _id,bool _akcja,unsigned long _startAkcji, unsigned long _czasTrwaniaAkcji=0)
        {
            sekcjaId=_id; akcja=_akcja;startAkcji=_startAkcji;czasTrwaniaAkcji=_czasTrwaniaAkcji;
        };
        uint8_t getSekwencjaId(){return sekwencjaId;};
        uint8_t getSekcjaId(){return sekcjaId;};
        bool getAkcja(){return akcja;};
        unsigned long getCzasTrwaniaAkcji(){return czasTrwaniaAkcji;};
        unsigned long getStartAkcji(){return startAkcji;};
        uint8_t getProgramId(){return programId;};

        String getSekwencjaCSVString();
        bool parseSekwencjaFromJson(String json,uint8_t _id=ID_SEKWENCJI_NIEZNANE);
        int setSekwencjaFromCSV(String csv);
        String getSekwencjaJsonString(bool dodajLastRun=false);
        bool copySekwencja(Sekwencja * source);

};

#endif