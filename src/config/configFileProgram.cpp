#include "configFileProgram.h"


int ConfigFileProgram::loadSekwencjeFromFile()
{
    Serial.println(__PRETTY_FUNCTION__);
    File file =LittleFS.open(filenameSekwencje, "r");
    String str="";
    int r=0;
   
    if (file) 
    {
        while(file.available())
        {
            str=file.readStringUntil('\n');//.readString();
          // Serial.println(str);
            Sekwencja *sek=new Sekwencja();
            r=sek->setSekwencjaFromCSV(str);
          /*  if(r<0) 
            {
                delete sek;  // ignorowanie blednych zapisow programow
                r=-1;
            }*/
            if(liczbaSekwencji<MAX_SEKWENCJE_SZT && r>=0)
            {
                sekwencjeTab[liczbaSekwencji]=sek;
                liczbaSekwencji++;
                Serial.printf("Wczytano sekwencja id: %d, sekcja: %d ->", sek->getSekwencjaId(),sek->getSekcjaId());
                Serial.println(str);
                Serial.println(sek->getSekwencjaJsonString());
            }else
            {
                Serial.println(F("ERROR. MAX liczba sekwencji"));
                delete sek;
                r=-2;
            }
        }
        file.close();
    }else r=-3;
    calcCzasTrwaniaProgramow();
    return r; 
}
int ConfigFileProgram::saveSekwencjeToFile()
{
    Serial.println(__PRETTY_FUNCTION__);
    bool wykryteZmiany=false;
    int r=0;
    if(!LittleFS.exists(filenameSekwencje)) 
    {
        Serial.println(F("brak pliku sekwencje"));
        wykryteZmiany=true;
    }
    
    /********** wykrywanie zmian **************/    
        File file =LittleFS.open(filenameSekwencje, "r");
        String str="";
        String str2="";
        
        if (file) 
        {
            Serial.println(F("Plik sekwencje otwarty - porownanie"));
            int i=0;
            while(file.available())
            {
                str=file.readStringUntil('\n');//.readString();
                if(i>= liczbaSekwencji)
                {
                    wykryteZmiany=true;
                    break;
                }
                str2=sekwencjeTab[i]->getSekwencjaCSVString();
                Serial.print(F("str  "));Serial.println(str);
                Serial.print(F("str2 "));Serial.println(str2);
                if(!str.equals(str2))
                {
                    Serial.print(F("-- Wykryte zmiany w id: "));Serial.println(i);
                    wykryteZmiany=true;
                    break;
                }
                i++;
            }
            if(i==0)// pusty plik
            {
                wykryteZmiany=true;
            }
        }
        file.close();
        /********** wykrywanie zmian koniec**************/
        if(wykryteZmiany)
        {
            Serial.println(F("Poczatek zapisu"));
            File f=LittleFS.open(filenameSekwencje,"w");
            if(f)
            {           
                Serial.println(F("-- Plik otwarty"));
                calcCzasTrwaniaProgramow();
                for(int j=0;j<liczbaSekwencji;j++)
                {   
                    Serial.print(F("--- Zapis: "));
                    str=sekwencjeTab[j]->getSekwencjaCSVString();
                    f.println(str);
                    Serial.println(str);
                }
                return 0;
            }else 
            { 
                Serial.println(F("-- err, zapisu sekwencji"));
                return -1;
            }
            f.close();
        }else
        { 
            Serial.println("Brak zmian, nie zapisuje sekwencji");
            return 1;
        }
    return r;
}   

String ConfigFileProgram::getSekwencjeJsonString(bool dodajLastRunSekwencji)
{
    Serial.print(__PRETTY_FUNCTION__);
   // String ret="{\"tag\":\""+String(TAG_CONFIG_FILE_SEKWENCJE)+"\",\"Sekwencje\":[";
     String ret="[";
    
    for(int i=0;i<liczbaSekwencji;i++)
    { 
        ret+= String(sekwencjeTab[i]->getSekwencjaJsonString(dodajLastRunSekwencji));
        if(i<liczbaSekwencji-1)ret+=",";
    }
    ret+="]"; Serial.print(": ");Serial.println(ret);
    return ret;
}
bool ConfigFileProgram::addSekwencja(String json)
{
    Serial.println(__PRETTY_FUNCTION__);
    if(liczbaSekwencji>= MAX_SEKWENCJE_SZT) 
    {
        Serial.println(F("Brak miejsca na nowa sekwencje"));
        return false;
    }
    Sekwencja* sek=new Sekwencja();
    int pierwszeWolneId=getFirstEmptySekwencjaId();
    bool r=sek->parseSekwencjaFromJson(json,pierwszeWolneId);
    Serial.println(sek->getSekwencjaCSVString());
    if(r)
    {   
        sekwencjeTab[liczbaSekwencji]=sek;
        Serial.println(F("Sekwencje dodano"));
        liczbaSekwencji++;
                  
    }else
    {
        Serial.println(F("Bledny json sekwencja"));
        delete sek;
        return false;
    }         
    return true;
}   
bool ConfigFileProgram::addSekwencjaAndSaveFile(String json)
{
    Serial.println(__PRETTY_FUNCTION__);
    if(addSekwencja(json))
    {
        return saveSekwencjeToFile()==0? true:false;
        
    }else 
    {
        return false;
    }
}
int  ConfigFileProgram::getFirstEmptySekwencjaId()
{
    Serial.print(__PRETTY_FUNCTION__);
    int r=0;
    while(true)
    {
        int i=getSekwencjaById(r);
        if(i==ID_SEKWENCJI_NIEZNANE)
        {
            Serial.printf("id: %d, wolne\n",r);
            break;
        }
        Serial.printf("id: %d, jest na indeksie: %d\n",r,i);
        r++;
    }
    Serial.printf("zwracam id: %d\n",r);
    return r;
}
uint8_t  ConfigFileProgram::getSekwencjaById(uint8_t id)
{
    uint8_t r=ID_SEKWENCJI_NIEZNANE;
    Serial.print(__PRETTY_FUNCTION__);Serial.println(id);
    for(int i=0;i<liczbaSekwencji;i++)
    { 
        if(sekwencjeTab[i]->getSekwencjaId()==id)
        {
            r = i;
            break;
        }
    }
    Serial.print(F(", index: "));Serial.println(r);
    return r;
}
bool ConfigFileProgram::changeSekwencjaFromJsonStringAndSaveFile(String json)
{
    Serial.println(__PRETTY_FUNCTION__);
    if(liczbaSekwencji>= MAX_SEKWENCJE_SZT) 
    {
        Serial.println(F("Sekwencja poza zakresem"));
        return false;
    }
    Sekwencja *tmp=new Sekwencja();
    if( tmp->parseSekwencjaFromJson(json))
    {
        uint8_t x=getSekwencjaById(tmp->getSekwencjaId());
        if(x==ID_SEKWENCJI_NIEZNANE)
        {
            Serial.printf("Sekwencja id: %d, nie istnieje\n",x);
            delete tmp;
            return false;        
        }
        sekwencjeTab[x]->copySekwencja(tmp);
    }
    delete tmp;
    return saveSekwencjeToFile()==0;
}
bool ConfigFileProgram::delSekwencjaFromJsonString(String json)
{
    Serial.println(__PRETTY_FUNCTION__);
    StaticJsonDocument<JSON_SIZE_PROGRAM> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
         Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
         return false;
    }else
    { 
       int  idSek=doc["sekwencjaId"] | MAX_SEKWENCJE_SZT;
       return delSekwencja(idSek);
    }
}
bool ConfigFileProgram::delSekwencja(uint8_t id)
{
    Serial.printf("ConfigFileProgram::delSekwencja id: %d wszystkich %d\n",id,liczbaSekwencji);
    Serial.println(getSekwencjeJsonString());
    int index=getSekwencjaById(id);
    if(index>=MAX_SEKWENCJE_SZT) 
    {
        Serial.println(F("Blad, sekwencja poza zakresem."));
        return false;
    }
    Serial.print(F("Przed Mem: ")); Serial.println(ESP.getFreeHeap());
    Sekwencja * sekUsun=sekwencjeTab[index];
    for(int i=index;i<liczbaSekwencji-1;i++)
    {
        sekwencjeTab[i]=sekwencjeTab[i+1];
    }
    Serial.println(getSekwencjeJsonString());
    //delete programyTab[liczbaProgramow];
    liczbaSekwencji--;
    delete sekUsun;
    Serial.print(F("Po Mem: ")); Serial.println(ESP.getFreeHeap());
    return saveSekwencjeToFile() == 0 ? true:false;
    
}

/********************************* programy ********************************/

int ConfigFileProgram::loadProgramsFromFile()
{
    Serial.println(__PRETTY_FUNCTION__);
    File file =LittleFS.open(filenameProgramy, "r");
    String str="";
    int r=0;
    if (file) 
    {
        while(file.available())
        {
            str=file.readStringUntil('\n');//.readString();
            Serial.println(str);
            Program *prog=new Program();
            r=prog->setProgramFromCSV(str);
           /* if(r<0) 
            {
                delete prog;  // ignorowanie blednych zapisow programow
                r=-1;
            }*/
            if(liczbaProgramow<MAX_PROGRAM_SZT && r>=0)
            {
                programyTab[liczbaProgramow]=prog;
                liczbaProgramow++;
                Serial.printf("Wczytano program id: %d, %s ", prog->id,prog->nazwa.c_str());
                Serial.println(str);
                Serial.println(prog->getProgramJsonString());
            }else
            {
                Serial.println(F("ERROR. MAX liczba programow"));
                delete prog;
                r=-2;
            }
        }
        file.close();
    }else r=-3;

    return r;
};
int ConfigFileProgram::saveProgramsToFile()
{
    Serial.println(__PRETTY_FUNCTION__);
    bool wykryteZmiany=false;
    int r=0;
    if(!LittleFS.exists(filenameProgramy)) 
    {
        Serial.println(F("brak pliku programy"));
        wykryteZmiany=true;
    }
    
    /********** wykrywanie zmian **************/    
        File file =LittleFS.open(filenameProgramy, "r");
        String str="";
        String str2="";
        
        if (file) 
        {
            Serial.println(F("Plik programy otwarty - porownanie"));
            int i=0;
            while(file.available())
            {
                str=file.readStringUntil('\n');//.readString();
                if(i>= liczbaProgramow)
                {
                    wykryteZmiany=true;
                    break;
                }
                str2=programyTab[i]->getProgramCSVString();
                Serial.print(F("str  "));Serial.println(str);
                Serial.print(F("str2 "));Serial.println(str2);
                if(!str.equals(str2))
                {
                    Serial.print(F("-- Wykryte zmiany w id: "));Serial.println(i);
                    wykryteZmiany=true;
                    break;
                }
                i++;
            }
            if(i==0)// pusty plik
            {
                wykryteZmiany=true;
            }
        }
        file.close();
        /********** wykrywanie zmian koniec**************/
        if(wykryteZmiany)
        {
            Serial.println(F("Poczatek zapisu"));
            File f=LittleFS.open(filenameProgramy,"w");
            if(f)
            {           
                Serial.println(F("-- Plik otwarty"));
                for(int j=0;j<liczbaProgramow;j++)
                {   
                    Serial.print(F("--- Zapis: "));
                    str=programyTab[j]->getProgramCSVString();
                    f.println(str);
                    Serial.println(str);
                }
                return 0;
            }else 
            { 
                Serial.println(F("-- err, zapisu programu"));
                return -1;
            }
            f.close();
        }else
        { 
            Serial.println("Brak zmian, nie zapisuje programow");
            return 1;
        }
    return r;
};


String ConfigFileProgram::getProgramyJsonString(bool dodajLastRunProgramu)
{
    Serial.print(__PRETTY_FUNCTION__);
  //  String ret="{\"tag\":\""+String(TAG_CONFIG_FILE_PROGRAMY)+"\",\"Programy\":[";
     String ret="[";
    
    for(int i=0;i<liczbaProgramow;i++)
    { 
        ret+= String(programyTab[i]->getProgramJsonString(dodajLastRunProgramu));
        if(i<liczbaProgramow-1)ret+=",";
    }
    ret+="]"; 
    Serial.print(": ");Serial.println(ret);
    return ret;
}


bool ConfigFileProgram::addProgramAndSaveFile(String json)
{
    Serial.println(__PRETTY_FUNCTION__);
    if(addProgram(json))
    {
        return saveProgramsToFile()==0? true:false;
        
    }else 
    {
        return false;
    }
}
int ConfigFileProgram::getFirstEmptyProgramId()
{
    Serial.print(__PRETTY_FUNCTION__);
    int r=0;
    while(true)
    {
        int i=getProgramById(r);
        if(i==ID_PROGRAMU_NIEZNANE)
        {
            Serial.printf("id: %d, wolne\n",r);
            break;
        }
        Serial.printf("id: %d, jest na indeksie: %d\n",r,i);
        r++;
    }
    Serial.printf("zwracam id: %d\n",r);
    return r;
}
/* zwraca index programu o zadanym id */
uint8_t ConfigFileProgram::getProgramById(uint8_t id)
{
    uint8_t r=ID_PROGRAMU_NIEZNANE;
     Serial.print(__PRETTY_FUNCTION__);Serial.println(id);
    for(int i=0;i<liczbaProgramow;i++)
    { 
        if(programyTab[i]->id==id)
        {
            r = i;
            break;
        }
    }
    Serial.print(F(", index: "));Serial.println(r);
    return r;
}
bool ConfigFileProgram::changeProgramFromJsonStringAndSaveFile(String json)
{
    Serial.println(__PRETTY_FUNCTION__);
    if(liczbaProgramow>= MAX_PROGRAM_SZT) 
    {
        Serial.println(F("Program poza zakresem"));
        return false;
    }
    Program *tmp=new Program();
    if( tmp->parseProgramFromJson(json))
    {
        uint8_t x=getProgramById(tmp->id);
        if(x==ID_PROGRAMU_NIEZNANE)
        {
            Serial.printf("Program id: %d, nie istnieje\n",x);
            delete tmp;
            return false;        
        }
        programyTab[x]->copyProgram(tmp);
    }
    delete tmp;
    return saveProgramsToFile()==0;
}
bool ConfigFileProgram::addProgram(String json)
{
            Serial.println(__PRETTY_FUNCTION__);
            if(liczbaProgramow>= MAX_PROGRAM_SZT) 
            {
                Serial.println(F("Brak miejsca na nowy program"));
                return false;
            }
            Program* prog=new Program();
            int pierwszeWolneId=getFirstEmptyProgramId();
            bool r=prog->parseProgramFromJson(json,pierwszeWolneId);
            Serial.println(prog->getProgramCSVString());
            if(r)
            {   
                programyTab[liczbaProgramow]=prog;
                Serial.println(F("Program dodano"));
                liczbaProgramow++;
                  
            }else
            {
                Serial.println(F("Bledny json program"));
                delete prog;
                return false;
            }         
            return true;
};
bool ConfigFileProgram::delProgramFromJsonString(String json)
{
    Serial.println(__PRETTY_FUNCTION__);
    StaticJsonDocument<JSON_SIZE_PROGRAM> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
         Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
         return false;
    }else
    { 
       int  idProg=doc["id"] | MAX_PROGRAM_SZT;
       return delProgram(idProg);
    }
}
bool ConfigFileProgram::delProgram(uint8_t id)
{
    Serial.printf("ConfigFileProgram::delProgram id: %d wszystkich %d\n",id,liczbaProgramow);
    Serial.println(getProgramyJsonString());
    int index=getProgramById(id);
    if(index>=MAX_PROGRAM_SZT) 
    {
        Serial.println(F("Blad, program poza zakresem."));
        return false;
    }
    Serial.print(F("Przed Mem: ")); Serial.println(ESP.getFreeHeap());
    Program * progUsun=programyTab[index];
    for(int i=index;i<liczbaProgramow-1;i++)
    {
        programyTab[i]=programyTab[i+1];
    }
    Serial.println(getProgramyJsonString());
    //delete programyTab[liczbaProgramow];
    liczbaProgramow--;
    delete progUsun;
    Serial.print(F("Po Mem: ")); Serial.println(ESP.getFreeHeap());
    return saveProgramsToFile() == 0 ? true:false;
    
}
void ConfigFileProgram::calcCzasTrwaniaProgramow()
{
    for(int i=0; i< liczbaProgramow;i++)
    {
        unsigned long c=0;
        unsigned long cTmp=0;
        for(int k=0;k< liczbaSekwencji;k++)
        {
            //czy sekwencja nalezy do programu
            if(sekwencjeTab[k]->getProgramId()==programyTab[i]->id)
            {
                // c znajdz max(startAkcji+czasTrwaniaAkcji)  
                cTmp=sekwencjeTab[k]->getStartAkcji()+sekwencjeTab[k]->getCzasTrwaniaAkcji();
                if(cTmp > c)
                {
                    c=cTmp;
                }
            }
            
        }
        programyTab[i]->czasTrwaniaProgramu=c;
    }
};