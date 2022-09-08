#ifndef SEKWENCJA_H
#define SEKWENCJA_H
#include <Arduino.h>


/* sekwencja programu inaczej pojedynczy krok*/
class Sekwencja
{

        uint8_t sekcjaId;
        bool akcja;
        unsigned long czasTrwaniaAkcji; // gdy czas==0 akcja nie powoduje antyakcji
        unsigned long startAkcji; //start akcji 
    public:
        Sekwencja(){};
        void setSekwencja(uint8_t _id,bool _akcja,unsigned long _startAkcji, unsigned long _czasTrwaniaAkcji=0)
        {
            sekcjaId=_id; akcja=_akcja;startAkcji=_startAkcji;czasTrwaniaAkcji=_czasTrwaniaAkcji;
        };
        uint8_t getSekcjaId(){return sekcjaId;};
        bool getAkcja(){return akcja;};
        unsigned long getCzasTrwaniaAkcji(){return czasTrwaniaAkcji;};
        unsigned long getStartAkcji(){return startAkcji;};

};

#endif