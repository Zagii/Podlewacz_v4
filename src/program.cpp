#include "program.h"

/*void Program::addSekwencje(String json)
{

};*/
int Program::dodajGodzine(int g)
{
    if(liczbaGodzin<MAX_ILOSC_URUCHAMIANIA_PROGRAMU_DZIENNIE)
    {
        godzinyTab[liczbaGodzin]=g;
        liczbaGodzin++;
        return 0;
    }else
    {
        return -1; //za duzo godzin
    }
};
String Program::getProgramCSVString()
{
    String ret=String(id)+String("|")+String(nazwa)+String("|");
    for(int i=0;i<7;i++) { ret+= dni[i] ? "1" : "0";}
    ret+=String("|")+ String(aktywny ? "1" : "0");
    for(int i=0;i<liczbaGodzin;i++) { ret+= String("|")+String(godzinyTab[i]);}
    return ret;
};
int Program::setProgramFromCSV(String str)
{
    String pole="";
   // int liczbaPol = 0;
    int nrPola = 0;
    while (str.length() > 0)
    {
        int index = str.indexOf('|');
        if (index == -1) // No space found
        {
            // strs[liczbaPol++] = str;
            dodajGodzine(str.toInt());
            break;
        }
        else
        {
            //"id":1,"nazwa":"program","dni":"1010101","aktywny":1,"godziny":[800,1130,1400,1515]
            // program|1010101|1|800;1130;1400;1515\n
            pole = str.substring(0, index);
            switch (nrPola)
            {
            case 0: //id
                id = pole.toInt();
                break;
            case 1: // nazwa
                nazwa = pole;
                break;
            case 2: // dni
                if(pole.length()!=7)
                {
                    Serial.println("Bledne dni programu");
                    return -1;
                }
                for(int i=0;i<7;i++) { dni[i]=pole.charAt(i)=='1'?true:false;  }
                break;
            case 3: // aktywny
                aktywny = pole.charAt(0)=='1'?true:false;
                break;
            default: // lista godzin programow
                    dodajGodzine(pole.toInt());
                break;
            }
            // strs[liczbaPol++] = pole
            str = str.substring(index + 1);
        }
        nrPola++;
    }
    return id;
};
/* **********************************************************************
************ parsuje stringa z konfiguracja sekcji na pola
** wejście: jsonConfig
** return: id sekcji odczytana z jsonConfig lub -1 jesli id nie podano
************************************************************************** */
int Program::setProgramFromJson(String jsonConfig)
{
    Serial.println(__PRETTY_FUNCTION__);
    Serial.println(jsonConfig);
    StaticJsonDocument<JSON_SIZE_PROGRAM> doc;
    DeserializationError error = deserializeJson(doc, jsonConfig);
    if (error)
    {
        Serial.println("JSON de-serialization failed: " + String(error.c_str()));
        id = PROGRAM_ID_BRAK;
        nazwa = "Program";
        for(int i=0;i<7;i++) { dni[i]=false;}
        aktywny=false;
        liczbaGodzin=0;
        lastProgramRun=0; 
        return PROGRAM_ID_BLAD;
    }
    else
    {
        id = doc["programId"] | PROGRAM_ID_BRAK;
        nazwa = doc["nazwa"] | String("Program");
        String d=doc["dni"] | "0000000";
        if(d.length()!=7)
        {
            Serial.println("Bledne dni programu");
            return false;
        }
        for(int i=0;i<7;i++) { dni[i]=d.charAt(i)=='1'?true:false;  }

        lastProgramRun=0; //data w sekundach od ostatnio uruchomionego programu
        aktywny= doc["aktywny"] | false; //)==1 ? true : false ; // czy program jest aktywny
        
        liczbaGodzin=0;
        if(doc.containsKey("godziny"))
        {
            JsonArray godzinyArr= doc["godziny"].as<JsonArray>();
            liczbaGodzin=godzinyArr.size();
            for(int i=0; i<liczbaGodzin; i++)
            {
              godzinyTab[i]=godzinyArr[i];
            }   
        }
    }
    return id;
};
/*
bool Program::parseProgramFromJson(String json, uint8_t _id)
{
     
    Serial.println(__PRETTY_FUNCTION__);//Serial.println(filename);
    StaticJsonDocument<JSON_SIZE_PROGRAM> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
         Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
         return false;
    }else
    { 
         if(_id != ID_PROGRAMU_NIEZNANE)
        {
            id=_id;
        }
        else{
            id=doc["programId"] | _id;
        }
        
        nazwa=doc["nazwa"]| String("prog");
   
        String d=doc["dni"] | "0000000";
        if(d.length()!=7)
        {
            Serial.println("Bledne dni programu");
            return false;
        }
        for(int i=0;i<7;i++) { dni[i]=d.charAt(i)=='1'?true:false;  }

        lastProgramRun=0; //data w sekundach od ostatnio uruchomionego programu
        aktywny= doc["aktywny"] | false; //)==1 ? true : false ; // czy program jest aktywny
        
        liczbaGodzin=0;
        if(doc.containsKey("godziny"))
        {
            JsonArray godzinyArr= doc["godziny"].as<JsonArray>();
            //int i=0;
            liczbaGodzin=godzinyArr.size();
            for(int i=0; i<liczbaGodzin; i++)
            //for (JsonObject g : godzinyArr)
            {
              //  godzinyList.Add(g);
              godzinyTab[i]=godzinyArr[i];
              //i++;
            }
           
        }
    }
    return true;
}*/

String Program::getProgramJsonString(bool dodajLastRun)
{

    String ret=String("{\"programId\":")+String(id)+String(",\"nazwa\":\"")+String(nazwa)+String("\",\"dni\":\"");
    for(int i=0;i<7;i++) { ret+= dni[i] ? "1" : "0"; }
    ret+=String("\",\"aktywny\":")+ String(aktywny ? "1" : "0");
    ret+=String(",\"godziny\":[");
    for(int i=0;i<liczbaGodzin;i++) { ret+= String(godzinyTab[i]);
        if(i<liczbaGodzin-1) ret+=String(",");
    }
    ret+="]";
    if(dodajLastRun)
    {
        ret+=String(",\"lastRun\":")+String(lastProgramRun);
    }
    ret+="}";
    return ret;
};
bool Program::copyProgram(Program * source)
{
    id=source->id;
    nazwa=source->nazwa;
    aktywny=source->aktywny;
    for(int i=0;i<7;i++)
        dni[i]=source->dni[i];
    liczbaGodzin=source->liczbaGodzin;
    for(int i=0;i<liczbaGodzin;i++)
        godzinyTab[i]=source->godzinyTab[i];
    return true;
}