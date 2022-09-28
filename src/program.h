#ifndef PROGRAM_H
#define PROGRAM_H
#include <ArduinoJson.h>
//#include <ListLib.h>
//#include "sekwencja.h"


#define JSON_SIZE_PROGRAM 256


//#define PROGRAM_TYP_DNI 0 //program aktywny tylko w wybrane dni tygodnia
//#define PROGRAM_TYP_PARZYSTE 1 //program aktywny tylko w parzyste dni miesiaca
//#define PROGRAM_TYP_NIEPARZYSTE 2 //program aktywny tylko w nieparzyste dni miesiąca

#define MAX_ILOSC_URUCHAMIANIA_PROGRAMU_DZIENNIE 8
#define MAX_ILOSC_SEKWENCJI_W_PROGRAMIE 50

#define ID_PROGRAMU_NIEZNANE 254
/* 
    program uruchamiany jest w konkretne dni o konkretnych godzinach
    program zawiera sekwencje czyli czasy uruchomienia wybranych sekcji
    */
class Program 
{   
    
    bool czyUruchomiony=false;
    public:
  //  uint8_t typ;
        String nazwa="";
        uint8_t id=ID_PROGRAMU_NIEZNANE; //id programu
        bool dni[7]; // w jakich dniach tygodnia program jest uruchamiany 0-nd,1-pn, ..6-sb
        unsigned long lastProgramRun=0; //data w sekundach od ostatnio uruchomionego programu
        bool aktywny=false; // czy program jest aktywny
        int godzinyTab[MAX_ILOSC_URUCHAMIANIA_PROGRAMU_DZIENNIE]; // zapis w formacie zulu hhmm
        uint8_t liczbaGodzin=0;

        unsigned long czasTrwaniaProgramu = 0;
      
   
        Program(){};
        ~Program(){   };
        int dodajGodzine(int g);
        String getProgramCSVString();
        bool parseProgramFromJson(String json,uint8_t _id=ID_PROGRAMU_NIEZNANE);
        int setProgramFromCSV(String csv);
        String getProgramJsonString(bool dodajLastRun=false);
        bool copyProgram(Program * source);
        void setCzasTrwaniaProgramu(unsigned long dt){czasTrwaniaProgramu=dt;}
        bool czyDzienProgramu(uint8_t nrDnia)
        {
          if(nrDnia>6)return false;
          return dni[nrDnia];
        };
        bool czyGodzinaStartuProgramu(int hhmm)
        {
          for(int i=0;i<liczbaGodzin;i++)
          {
            if(godzinyTab[i]==hhmm)
            {
              return true;
            }
          }
          return false;
        };
        bool czyProgramUruchomiony(){return czyUruchomiony;};
        void startProgram(unsigned long czasUruchomieniaSekundy){czyUruchomiony=true;lastProgramRun=czasUruchomieniaSekundy;};
        void stopProgram(){czyUruchomiony=false;}
        

};
#endif