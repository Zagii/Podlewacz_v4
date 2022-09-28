#include "configFileSekcje.h"

void ConfigFileSekcje::begin(HardwareOutput *_h/*, ApiServer *_api*/)
{
    hardwareOutput=_h;
    loadSekcjeFromFile();
};

int ConfigFileSekcje::loadSekcjeFromFile()
{
     Serial.println(__PRETTY_FUNCTION__);
    File file =LittleFS.open(filename, "r");
    String str="";
    int r=0;
    if (file) 
    {
        while(file.available())
        {
            str=file.readStringUntil('\n');//.readString();
            Serial.println(str);
            Sekcja *sek=new Sekcja();
            r=sek->setSekcjaFromJson(str);
           
            if(liczbaSekcji<MAX_LICZBA_SEKCJI && r>=0)
            {
                sekcje[liczbaSekcji]=sek;
                liczbaSekcji++;
                Serial.printf("Wczytano sekcja id: %d, %s ", sek->id,sek->nazwa.c_str());
                Serial.println(str);
                Serial.println(sek->getSekcjaJsonString());
            }else
            {
                Serial.println(F("ERROR. odczyt sekcji"));
                delete sek;
                r=-2;
            }
        }
        file.close();
    }else r=-3;

    return r;
}
void ConfigFileSekcje::loop()
{
    for(int i=0;i<liczbaSekcji;i++)
    {
        sekcje[i]->loop();
    }
}
/*bool ConfigFileSekcje::parseFile(String json)
{
            Serial.println("Parsowanie pliku sekcji");//Serial.println(filename);
            StaticJsonDocument<JSON_SIZE_SEKCJE> doc;
            DeserializationError error = deserializeJson(doc, json);
            if (error) {
                     Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
                     return false;
                }
            else
            {
                //liczbaSekcji=doc["liczbaSekcji"] | 0;
                if(!doc.containsKey("Sekcje"))
                {
                    
                    Serial.println("Brak obiektu Sekcje w konfiguracji pliku");
                   // liczbaSekcji=0;
                }else
                {
                    JsonArray sekcjeTab= doc["Sekcje"].as<JsonArray>();
                
                    liczbaSekcji=sekcjeTab.size();
                    Serial.print("Zapisanych w pliku: "); Serial.print(liczbaSekcji); Serial.println(" sekcji");
                    int i=0;
                    for (JsonObject repo : sekcjeTab)
                    {
                        //const char* name = repo["name"];
                     //   StaticJsonDocument<JSON_SIZE> docSekcja;
                        String sekcjaStr="";
                        serializeJson(repo, sekcjaStr);
                     //   serializeJson()
                        Serial.println("Serialized sekcja:");
                        Serial.println(sekcjaStr);
                        if(i<MAX_LICZBA_SEKCJI)
                        {
                            sekcje[i]=new Sekcja();
                            sekcje[i]->begin(hardwareOutput,i,sekcjaStr);
                            i++;
                        }else
                        {
                            Serial.println("Przekroczono max liczbe sekcji w konfiguracji pliku");
                            return false;
                        }
                    }
                }
                

            }
            
            return true;
};*/
/*
bool ConfigFileSekcje::parseJsonSekcja(String json)
{
    Serial.println("Parsowanie jednej sekcji"); // Serial.println(filename);
        
        Serial.println(sekcjaStr);
        Sekcja *nowaSekcja = new Sekcja();
        int i =getFirstEmptySekcjaId();
        bool czyEdycja=nowaSekcja->begin(hardwareOutput, i, sekcjaStr);
        
        if(czyEdycja)
        {

            int x=getSekcjaById();
            
            return true;
        }else
        {// nowa sekcja
            if (liczbaSekcji + 1 < MAX_LICZBA_SEKCJI)
            {

                liczbaSekcji++;
            }else
            {
                Serial.println("Przekroczono max liczbe sekcji w konfiguracji pliku");
                return false;
            }
        }
};
*/

bool ConfigFileSekcje::setSekcjaConfig(String json)
{
    Serial.println("ConfigFileSekcje::setSekcjaConfig");
    //todo podwojne parsowanie nie jest zbyt optymalne do zastanowienia na pozniej
    StaticJsonDocument<JSON_SIZE_SEKCJE> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
                     Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
                     return false;
    }else
    {
       
           // parseFile(json);

            return saveSekcjeToFile();              
       
    }
};
/* 
bool ConfigFileSekcje::setSekcjeConfig(String json)
{
    Serial.println("ConfigFileSekcje::setSekcjeConfig");
    //todo podwojne parsowanie nie jest zbyt optymalne do zastanowienia na pozniej
    StaticJsonDocument<JSON_SIZE_SEKCJE> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
                     Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
                     return false;
    }else
    {
        if(doc.containsKey("Sekcje"))
        {
            for(int i=0;i<liczbaSekcji;i++)
            {
                if(sekcje[i])
                {
                    Serial.print("  usuwam sekcje: ");Serial.println(i);
                    delete sekcje[i];
                }
            }
            parseFile(json);
            return saveToFile();              
        }else
        {
            Serial.println("Brak obektu sekcji w json");
            return false;
        }
        return false;
    }

    
};*/


bool ConfigFileSekcje::setSekcjeStan(String json)
{
    Serial.println("ConfigFileSekcje::setSekcjeStan");
    //todo podwojne parsowanie nie jest zbyt optymalne do zastanowienia na pozniej


    StaticJsonDocument<JSON_SIZE_SEKCJE> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
                     Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
                     return false;
    }else
    {
        if(doc.containsKey("Stany"))
        {
            JsonArray stanyTab= doc["Stany"].as<JsonArray>();
              
            uint8_t n =stanyTab.size();
            Serial.print("  Liczba polecen: "); Serial.print(n); Serial.println(" szt.");
            for (JsonObject repo : stanyTab)
            {
                uint8_t sekcjaId=repo["id"];
                if(sekcjaId>=liczbaSekcji || sekcjaId<0)
                {
                    Serial.println("Blad danych. Sekcja poza zakresem. Ignoruje");
                    continue;
                }
                uint8_t nowyStan=repo["stan"];
                long cz=repo["czas"] | 0;
                Serial.printf("-- zmiana sekcji: %d, na %d, czas: %ld\n",sekcjaId,nowyStan,cz);
                if(cz>0)
                {
                    sekcje[sekcjaId]->setStateForTime(nowyStan,cz);
                }else
                {
                    sekcje[sekcjaId]->setState(nowyStan);
                }
            }
        }
    }
    return true;
};

String ConfigFileSekcje::getSekcjeStan()
{
    String ret="";
    StaticJsonDocument<JSON_SIZE_SEKCJE> doc;
    JsonArray stany = doc.createNestedArray("stany");
    for(int i=0;i<liczbaSekcji;i++)
    {
        //JsonObject s = stany.createNestedObject();
        stany[i] = serialized(sekcje[i]->getStanJson());   
    }
    serializeJson(doc,ret);
    return ret;
};
int  ConfigFileSekcje::getSekcjaById(int id)
{
    int r=SEKCJA_ID_BRAK;
    Serial.print(__PRETTY_FUNCTION__);Serial.println(id);
    if(id==SEKCJA_ID_BRAK || id == SEKCJA_ID_BLAD)
        return id;
    for(int i=0;i<liczbaSekcji;i++)
    { 
        if(sekcje[i]->id==id)
        {
            r = i;
            break;
        }
    }
    Serial.print(F(", index: "));Serial.println(r);
    return r;
};
int  ConfigFileSekcje::getFirstEmptySekcjaId()
{
    Serial.print(__PRETTY_FUNCTION__);
    int r=0;
    while(true)
    {
        int i=getSekcjaById(r);
        if(i<0)
        {
            Serial.printf("id: %d, wolne\n",r);
            break;
        }
        Serial.printf("id: %d, jest na indeksie: %d\n",r,i);
        r++;
    }
    Serial.printf("zwracam id: %d\n",r);
    return r;
};


int ConfigFileSekcje::saveSekcjeToFile()
{
    Serial.println(__PRETTY_FUNCTION__);
    bool wykryteZmiany=false;
    int r=0;
    if(!LittleFS.exists(filename)) 
    {
        Serial.println(F("brak pliku sekcji"));
        wykryteZmiany=true;
    }
    
    /********** wykrywanie zmian **************/    
        File file =LittleFS.open(filename, "r");
        String str="";
        String str2="";
        
        if (file) 
        {
            Serial.println(F("Plik sekcji otwarty - porownanie"));
            int i=0;
            while(file.available())
            {
                str=file.readStringUntil('\n');//.readString();
                if(i>= liczbaSekcji)
                {
                    wykryteZmiany=true;
                    break;
                }
                str2=sekcje[i]->getSekcjaJsonString();
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
            File f=LittleFS.open(filename,"w");
            if(f)
            {           
                Serial.println(F("-- Plik otwarty"));
                for(int j=0;j<liczbaSekcji;j++)
                {   
                    Serial.print(F("--- Zapis: "));
                    str=sekcje[j]->getSekcjaJsonString();
                    f.println(str);
                    Serial.println(str);
                }
                return 0;
            }else 
            { 
                Serial.println(F("-- err, zapisu sekcji"));
                return -1;
            }
            f.close();
        }else
        { 
            Serial.println("Brak zmian, nie zapisuje sekcji");
            return 1;
        }
    return r;
};

bool ConfigFileSekcje::addSekcja(String json)
{
       Serial.println(__PRETTY_FUNCTION__);
            if(liczbaSekcji>= MAX_LICZBA_SEKCJI) 
            {
                Serial.println(F("Brak miejsca na nowa sekcje"));
                return false;
            }
            Sekcja* sek=new Sekcja();
            int r=sek->setSekcjaFromJson(json);
            Serial.println(sek->getSekcjaJsonString());
            if(r!=SEKCJA_ID_BLAD && r!= SEKCJA_ID_BRAK)
            {   
                sek->id=getFirstEmptySekcjaId();
                sekcje[liczbaSekcji]=sek;
                Serial.println(F("Sekcje dodano"));
                liczbaSekcji++;
                  
            }else
            {
                Serial.println(F("Bledny json sekcja"));
                delete sek;
                return false;
            }         
            return true;
};   
bool ConfigFileSekcje::addSekcjaAndSaveFile(String json)
{
    Serial.println(__PRETTY_FUNCTION__);
    if(addSekcja(json))
    {
        return saveSekcjeToFile()==0? true:false;
        
    }else 
    {
        return false;
    }
};      
String ConfigFileSekcje::getSekcjeJsonString()
{
    return "";
};
bool ConfigFileSekcje::setSekcjaStan(String json)
{
    Serial.println(__PRETTY_FUNCTION__);      
    StaticJsonDocument<JSON_SIZE_SEKCJE> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) 
    {
        Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
        return false;
    }else
    {
            uint8_t sekcjaId=doc["id"] | -1;
            uint8_t nowyStan=doc["stan"]| false;
            long cz=doc["czas"] | 0;
            Serial.printf("-- zmiana sekcji: %d, na %d, czas: %ld\n",sekcjaId,nowyStan,cz);
            return setSekcjaStan(sekcjaId,nowyStan,cz);            
    }
    return true;
};
bool ConfigFileSekcje::setSekcjaStan(uint8_t sekcjaId, bool stan, long secondsToSwitch)
{
    bool r = false;
    int i = getSekcjaById(sekcjaId);
    if (i > 0)
    {
        if (secondsToSwitch > 0)
        {
            sekcje[i]->setStateForTime(stan, secondsToSwitch);
        }
        else
        {
            sekcje[i]->setState(stan);
        }
        r = true;
    }
    else
    {
        r = false;
    }
    return r;
};
void ConfigFileSekcje::setOffSekcjeAll()
        {
             for(int i=0; i< liczbaSekcji; i++)
            {
                sekcje[i]->setOff();
            }
        };
bool ConfigFileSekcje::changeSekcjaFromJsonStringAndSaveFile(String json)
{
    Serial.println(__PRETTY_FUNCTION__);
    Sekcja *tmp=new Sekcja();
    if( tmp->setSekcjaFromJson(json))
    {
        uint8_t x=getSekcjaById(tmp->id);
        if(x<0)
        {
            Serial.printf("Sekcja id: %d, nie istnieje\n",x);
            delete tmp;
            return false;        
        }
        sekcje[x]->copySekcja(tmp);
    }
    delete tmp;
    return saveSekcjeToFile()==0;
};
bool ConfigFileSekcje::delSekcjaFromJsonString(String json)
{
    Serial.println(__PRETTY_FUNCTION__);
    StaticJsonDocument<JSON_SIZE_SEKCJE> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
         Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
         return false;
    }else
    { 
       int  idSek=doc["id"] | SEKCJA_ID_BRAK;
       return delSekcja(idSek);
    }
};
bool ConfigFileSekcje::delSekcja(uint8_t id)
{
    Serial.printf("ConfigFileSekcja::delSekcja id: %d wszystkich %d\n",id,liczbaSekcji);
    Serial.println(getSekcjeJsonString());
    int index=getSekcjaById(id);
    if(index==SEKCJA_ID_BLAD || index == SEKCJA_ID_BRAK) 
    {
        Serial.println(F("Blad, sekcja poza zakresem."));
        return false;
    }
    Serial.print(F("Przed Mem: ")); Serial.println(ESP.getFreeHeap());
    Sekcja * sekUsun=sekcje[index];
    for(int i=index;i<liczbaSekcji-1;i++)
    {
        sekcje[i]=sekcje[i+1];
    }
    Serial.println(getSekcjeJsonString());
    //delete programyTab[liczbaProgramow];
    liczbaSekcji--;
    delete sekUsun;
    Serial.print(F("Po Mem: ")); Serial.println(ESP.getFreeHeap());
    return saveSekcjeToFile() == 0 ? true:false;
};