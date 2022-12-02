#include "manager.h"



/* zwraca id programu do uruchomienia jesli w danej minucie zaplanowano jego start
    w przeciwnym wypadku zwracany jest BRAK_PROGRAMU*/
int Manager::sprawdzKtoryUruchomicProgram(int d,int hhmm)
{
    Serial.printf("Sprawdz programy-> dzien: %d, hhmm: %d ->",d,hhmm); 

    for(int i=0;i<config->programConf.liczbaProgramow;i++)
    {
        //Serial.print(i);
        if(!config->programConf.programyTab[i]->czyAktywny())
        {
            Serial.print("n ");
                      
            continue;
        }
        if(config->programConf.programyTab[i]->czyProgramUruchomiony())
        {   // mamy juz wczesniej uruchomiony program
           Serial.print("u ");
        }else
        {
            if(config->programConf.programyTab[i]->czyDzienProgramu(d))
            {
                
                if(config->programConf.programyTab[i]->czyGodzinaStartuProgramu(hhmm))
                {
                    Serial.print("S ");
                    return config->programConf.programyTab[i]->id;
                }
                Serial.print(". ");
            }else Serial.print("_ ");
        }
    }
     Serial.println(";");
    return BRAK_PROGRAMU;
};
void Manager::sprawdzSekwencje(unsigned long obecnaSekundaDzialaniaProgramu)
{
    for(int i=0; i< config->programConf.liczbaSekwencji ; i++)
    {
            if( config->programConf.sekwencjeTab[i]->getProgramId() == uruchomionyProgramId)
            {   

                unsigned long startKorekta =  (unsigned long) (config->programConf.sekwencjeTab[i]->getStartAkcji()*korekta);
                unsigned long czasTrwaniaKorekta =  (unsigned long) (config->programConf.sekwencjeTab[i]->getCzasTrwaniaAkcji()*korekta);

                // sprawdz czy sekcja dotyczaca tej sekwencji ma odpowiedni stan
             //  if( config->programConf.sekwencjeTab[i]->testSekwencji(obecnaSekundaDzialaniaProgramu,
              // )
                // jesli nie to ustaw odpowiedni stan
                    if( startKorekta == obecnaSekundaDzialaniaProgramu)
                    {
                    
                        Serial.printf("%lu) sekwencja %d start, sekcjaId: %d, akcja: %d, startKor %lu, czasKor: %lu, korekta: %3.1f\n", 
                        obecnaSekundaDzialaniaProgramu,
                        config->programConf.sekwencjeTab[i]->getSekwencjaId(),
                        config->programConf.sekwencjeTab[i]->getSekcjaId(),
                        config->programConf.sekwencjeTab[i]->getAkcja(),
                        startKorekta,
                        czasTrwaniaKorekta,
                        korekta);
                        config->sekcjeConf.setSekcjaStan(config->programConf.sekwencjeTab[i]->getSekcjaId(),
                                                       config->programConf.sekwencjeTab[i]->getAkcja(),
                                                    czasTrwaniaKorekta);

                    }
            }
    }

}
void Manager::loop()
{
    
    unsigned long t=czas->getTimeInSecondsRTC();
    if(czasOstatniegoTestu != t)
    {
       // Serial.print(__PRETTY_FUNCTION__);Serial.print(" ");Serial.print(czasOstatniegoTestu);
        czasOstatniegoTestu=t;
        DateTime dt=DateTime(t);
        //String ret=String(dt.dayOfTheWeek())+"h"+String(dt.hour())+":"+String(dt.minute())+":"+String(dt.second());
        // 
        int d=dt.dayOfTheWeek();
        int hhmm=dt.hour()*100+dt.minute();
        
       
        // sprawdz czy mamy juz uruchomiony program
        if(uruchomionyProgramId!=BRAK_PROGRAMU)
        {
            
            //--- program x uruchiomiony wybierz sekwencje
            obecnaSekundaDzialaniaProgramu = czasOstatniegoTestu - config->programConf.programyTab[uruchomionyProgramIndex]->lastProgramRun;
            Serial.printf("Manager uruchomiony programId: %d, index: %d, czas: %lu\n",uruchomionyProgramId,uruchomionyProgramIndex,obecnaSekundaDzialaniaProgramu );
            sprawdzSekwencje(obecnaSekundaDzialaniaProgramu);
            //--- czy program x powinien sie zatrzymac
            if((unsigned long)(config->programConf.programyTab[uruchomionyProgramIndex]->czasTrwaniaProgramu*korekta) < obecnaSekundaDzialaniaProgramu)
            {
               stopProgram();
            }
        }else
        {   
            if(lastTestHhMM==hhmm){return;} //zakladamy uruchomienie tylko o pelnych minutach i tylko raz

            lastTestHhMM=hhmm;
            //zaden program nie uruchomiony sprawdz czy uruchomic jakis
            int progId=sprawdzKtoryUruchomicProgram(d,hhmm);

            //!!!kalkulowanie korakty todo!!!

            // w tej chwili powinien uruchomic się program
             if(progId!=BRAK_PROGRAMU) startProgram(progId,korekta,czasOstatniegoTestu);  
        }
    }// czas ostatniego testu
}
bool Manager::startProgram(uint8_t programId,  double _korekta, unsigned long czasStartu)
{
    Serial.printf("%s, %d, %3.1f, %lu\n",__PRETTY_FUNCTION__,programId,_korekta,czasStartu);
    if((programId!=BRAK_PROGRAMU)&&(programId!=ID_PROGRAMU_NIEZNANE))
    {   
        
        lastRunProgramId=programId;
        config->sekcjeConf.setOffSekcjeAll();
                if(czasStartu==0)czasStartu=czas->getTimeInSecondsRTC();
                uruchomionyProgramIndex=config->programConf.getProgramById(programId);
                if(uruchomionyProgramIndex==ID_PROGRAMU_NIEZNANE)
                {
                    //nieznane id programu
                    return false;
                }
                config->programConf.programyTab[uruchomionyProgramIndex]->startProgram(czasStartu);
                uruchomionyProgramId=programId;
                korekta=_korekta;
                Serial.printf("* Manager program %d start, %3.1f\n", uruchomionyProgramId,korekta);
                sprawdzSekwencje(0);
    }else
    {
        return false;
    }
    return true;
};
bool Manager::stopProgram()
{
    Serial.println(__PRETTY_FUNCTION__);
    if(uruchomionyProgramId!=BRAK_PROGRAMU)
    {
        config->programConf.programyTab[uruchomionyProgramIndex]->stopProgram();
        config->sekcjeConf.setOffSekcjeAll();
        Serial.printf("** Manager program %d stop\n", uruchomionyProgramId);
        uruchomionyProgramId=BRAK_PROGRAMU;
        obecnaSekundaDzialaniaProgramu=0;
        korekta=1;
        return true;
    }
    return false;
}

String Manager::getStatusJson()
{
  //  Serial.println(__PRETTY_FUNCTION__);
    String r=String("{\"czasOstatniegoTestu\":")+String(czasOstatniegoTestu);
     if(lastRunProgramId!=BRAK_PROGRAMU)
    {
        r+=String(",\"lastRunProgramId\":")+String(lastRunProgramId);
    }
    if(uruchomionyProgramId!=BRAK_PROGRAMU)
   {
     r+=String(",\"x\":")+String(korekta);
     r+=String(",\"uruchomionyProgramId\":")+String(uruchomionyProgramId);
     r+=String(",\"obecnaSekundaDzialaniaProgramu\":")+String(obecnaSekundaDzialaniaProgramu);
   }
    
    r+=String("}"); 
    return r;
}