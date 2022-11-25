#include "manager.h"



/* zwraca id programu do uruchomienia jesli w danej minucie zaplanowano jego start
    w przeciwnym wypadku zwracany jest BRAK_PROGRAMU*/
int Manager::sprawdzKtoryUruchomicProgram(int d,int hhmm)
{
    for(int i=0;i<config->programConf.liczbaProgramow;i++)
    {
        if(config->programConf.programyTab[i]->czyProgramUruchomiony())
        {   // mamy juz wczesniej uruchomiony program

        }else
        {
            if(config->programConf.programyTab[i]->czyDzienProgramu(d))
            {
                if(config->programConf.programyTab[i]->czyGodzinaStartuProgramu(hhmm))
                {
                    return config->programConf.programyTab[i]->id;
                }
            }
        }
    }
    return BRAK_PROGRAMU;
};
void Manager::sprawdzSekwencje(unsigned long obecnaSekundaDzialaniaProgramu)
{
    for(int i=0; i< config->programConf.liczbaSekwencji ; i++)
    {
            if( config->programConf.sekwencjeTab[i]->getProgramId() == uruchomionyProgramId)
            {
                    if( config->programConf.sekwencjeTab[i]->getStartAkcji() == obecnaSekundaDzialaniaProgramu)
                    {
                        Serial.printf("%lu) sekwencja %d start, sekcjaId: %d, akcja: %d, czas: %lu\n", 
                        obecnaSekundaDzialaniaProgramu,
                        config->programConf.sekwencjeTab[i]->getSekwencjaId(),
                        config->programConf.sekwencjeTab[i]->getSekcjaId(),
                        config->programConf.sekwencjeTab[i]->getAkcja(),
                        config->programConf.sekwencjeTab[i]->getCzasTrwaniaAkcji());
                        config->sekcjeConf.setSekcjaStan(config->programConf.sekwencjeTab[i]->getSekcjaId(),
                                                       config->programConf.sekwencjeTab[i]->getAkcja(),
                                                       config->programConf.sekwencjeTab[i]->getCzasTrwaniaAkcji());

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
            if(config->programConf.programyTab[uruchomionyProgramIndex]->czasTrwaniaProgramu < obecnaSekundaDzialaniaProgramu)
            {
               stopProgram();
            }
        }else
        {   
            //zaden program nie uruchomiony sprawdz czy uruchomic jakis
            int progId=sprawdzKtoryUruchomicProgram(d,hhmm);
            // w tej chwili powinien uruchomic się program
             if(progId!=BRAK_PROGRAMU) startProgram(progId,czasOstatniegoTestu);  
        }
    }// czas ostatniego testu
}
bool Manager::startProgram(uint8_t programId,unsigned long czasStartu)
{
    Serial.printf("%s, %d, %lu\n",__PRETTY_FUNCTION__,programId,czasStartu);
    if(programId!=BRAK_PROGRAMU)
    {   
        lastRunProgramId=programId;
        config->sekcjeConf.setOffSekcjeAll();
                if(czasStartu==0)czasStartu=czas->getTimeInSecondsRTC();
                uruchomionyProgramIndex=config->programConf.getProgramById(programId);
                config->programConf.programyTab[uruchomionyProgramIndex]->startProgram(czasStartu);
                uruchomionyProgramId=programId;
                Serial.printf("* Manager program %d start\n", uruchomionyProgramId);
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
        return true;
    }
    return false;
}

String Manager::getStatusJson()
{
    Serial.println(__PRETTY_FUNCTION__);
    String r=String("{\"czasOstatniegoTestu\":")+String(czasOstatniegoTestu);
     if(lastRunProgramId!=BRAK_PROGRAMU)
    {
        r+=String(",\"lastRunProgramId\":")+String(lastRunProgramId);
    }
    if(uruchomionyProgramId!=BRAK_PROGRAMU)
   {
     r+=String(",\"uruchomionyProgramId\":")+String(uruchomionyProgramId);
     r+=String(",\"obecnaSekundaDzialaniaProgramu\":")+String(obecnaSekundaDzialaniaProgramu);
   }
    
    r+=String("}"); 
    return r;
}