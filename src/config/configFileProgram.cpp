#include "configFileProgram.h"

int ConfigFileProgram::loadProgramsFromFile()
{
    Serial.println(__FUNCTION__);
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
            int r=prog->setProgramFromCSV(str);
            if(r<0) 
            {
                delete prog;  // ignorowanie blednych zapisow programow
                r=-1;
            }
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
    Serial.println(__FUNCTION__);
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
                Serial.println(str);
                Serial.println(str2);
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
/*
String ConfigFileProgram::prepareFile()
{   
    Serial.println("configFileProgram::prepareFile ");
    return getProgramyJsonString();
};*/

String ConfigFileProgram::getProgramyJsonString(bool dodajLastRunProgramu)
{
    Serial.print(__FUNCTION__);
    String ret="{\"tag\":\""+String(TAG_CONFIG_FILE_PROGRAMY)+"\",\"Programy\":[";
    
    for(int i=0;i<liczbaProgramow;i++)
    { 
        ret+= String(programyTab[i]->getProgramJsonString(dodajLastRunProgramu));
        if(i<liczbaProgramow-1)ret+=",";
    }
    ret+="]}"; Serial.print(": ");Serial.println(ret);
    return ret;
}
/*
bool ConfigFileProgram::parseFile(String json)
{
            Serial.println("Parsowanie pliku program");//Serial.println(filenameProgramy);
            StaticJsonDocument<JSON_SIZE> doc;
            DeserializationError error = deserializeJson(doc, json);
            if (error) {
                     Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
                     return false;
                }
            else
            {
                if(!doc.containsKey("Programy"))
                {
                    
                    Serial.println(F("Brak obiektu Programy w konfiguracji pliku"));
                    return false;
                   // liczbaSekcji=0;
                }else
                {
                    JsonArray pArr= doc["Programy"].as<JsonArray>();
                    for (JsonObject g : pArr)
                    {
                        String s="";
                        serializeJson(g,s);
                        addProgram(s);
                    }

                 //   liczbaSekcji=sekcjeTab.size();
                //    Serial.print("Zapisanych w pliku: "); Serial.print(liczbaSekcji); Serial.println(" sekcji");
                //    int i=0;
                 //   for (JsonObject repo : sekcjeTab)
                  //  {
                     
                  //  }
                }
                

            }
            
            return true;
};*/
/*
bool parseProgramFromCSV(String csv)
{

}*/

bool ConfigFileProgram::addProgramAndSaveFile(String json)
{
    Serial.println(__FUNCTION__);
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
    Serial.print(__FUNCTION__);
    int r=0;//liczbaProgramow;
    Serial.printf("liczbaProgramow: %d\n",liczbaProgramow);
   
    for(int k=0;k<liczbaProgramow;k++)
    {
        for(int i=0;i<MAX_PROGRAM_SZT;i++)
        {
            if(programyTab[k]->id==i)
            {
                // jest program o tym id
                Serial.printf("k: %d, id: %d, zajete, r= %d, zmiana r na: %d\n",k,i,r,r+1);
                 r++;
                break;
            }else
            {
                Serial.printf("k: %d, id: %d, wolne, r= %d\n",k,i,r);
                // nie ma takiego programu
                //if(i<r)
              //  {
                   
                    
                //}
            }
        }
    }
    Serial.printf("id: %d\n",r);
    return r;
}
/* zwraca index programu o zadanym id */
uint8_t ConfigFileProgram::getProgramById(uint8_t id)
{
    uint8_t r=ID_PROGRAMU_NIEZNANE;
     Serial.print(__FUNCTION__);Serial.print(id);
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
bool ConfigFileProgram::changeProgramFromJsonString(String json)
{
    Serial.println(__FUNCTION__);
    if(liczbaProgramow>= MAX_PROGRAM_SZT) 
    {
        Serial.println(F("Program poza zakresem"));
        return false;
    }
    Program *tmp=new Program();
    if( tmp->parseProgramFromJson(json))
    {
        uint8_t x=getProgramById(tmp->id);
        programyTab[x]->copyProgram(tmp);
    }
    delete tmp;
    return true;
}
bool ConfigFileProgram::addProgram(String json)
{
            Serial.println(__FUNCTION__);
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
    Serial.println(__FUNCTION__);
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
    delete programyTab[index];
    for(int i=index;i<liczbaProgramow-1;i++)
    {
        programyTab[i]=programyTab[i+1];
    }
    Serial.println(getProgramyJsonString());
    //delete programyTab[liczbaProgramow];
    liczbaProgramow--;
    Serial.print(F("Po Mem: ")); Serial.println(ESP.getFreeHeap());
    return saveProgramsToFile() == 0 ? true:false;
    
}