 #include "sekwencja.h"

String Sekwencja::getSekwencjaCSVString()
{
    String ret=String(sekwencjaId)+String("|")+String(sekcjaId)+String("|");
    ret+=String(akcja?1:0)+ String("|")+String(czasTrwaniaAkcji)+String("|")+String(startAkcji);
    return ret;
};
int Sekwencja::setSekwencjaFromCSV(String csv)
{
    String pole="";
    int nrPola = 0;
    while (csv.length() > 0)
    {
        int index = csv.indexOf('|');
        if (index == -1) // startAkcji
        {
            startAkcji=pole.toInt();
            break;
        }
        else
        {
            // 1|1|1|600|2123
            pole = csv.substring(0, index);
            switch (nrPola)
            {
            case 0: //sekwencjaId
                sekwencjaId = pole.toInt();
                break;
            case 1: // sekcjaId
                sekcjaId = pole.toInt();
                break;
            case 2: // akcja
                akcja=pole.charAt(0)=='1'? true:false;
                break;
            case 3: // czasTrwaniaAkcji
                czasTrwaniaAkcji = pole.toInt();
                break;
            default: // startAkcji
                    startAkcji=pole.toInt();
                break;
            }
            // strs[liczbaPol++] = pole
            csv = csv.substring(index + 1);
        }
        nrPola++;
    }
    return sekwencjaId;
};
bool Sekwencja::parseSekwencjaFromJson(String json,uint8_t _id)
{
    
    Serial.println(__PRETTY_FUNCTION__);
    StaticJsonDocument<JSON_SIZE_SEKWENCJA> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
         Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
         return false;
    }else
    { 
        sekwencjaId = doc["sekwencjaId"] | _id;
        sekcjaId=doc["sekcjaId"] | 254;
        akcja=(doc["akcja"] | 0)==1?true:false;
        czasTrwaniaAkcji=doc["czasTrwaniaAkcji"] | 0;
        startAkcji = doc["startAkcji"] | 0;          
    }
    return true;
}

String Sekwencja::getSekwencjaJsonString(bool dodajLastRun)
{
    Serial.println(__PRETTY_FUNCTION__);
    String ret=String("{\"sekwencjaId\":")+String(sekwencjaId)+String(",\"sekcjaId\":\"")+String(sekcjaId)+String("\",\"akcja\":\"");
    ret+=String(akcja ? "1" : "0");
    ret+=String(",\"czasTrwaniaAkcji\":")+String(czasTrwaniaAkcji)+String("\"startAkcji\":")+String(startAkcji);
    if(dodajLastRun)
    {
        ret+=String(",\"lastRun\":")+String(sekwencjaLastRun);
    }
    ret+="}";
    return ret;
}
bool Sekwencja::copySekwencja(Sekwencja * source)
{
    sekwencjaId=source->getSekwencjaId();
    sekcjaId=source->getSekcjaId();
    akcja=source->getAkcja();
    czasTrwaniaAkcji=source->getCzasTrwaniaAkcji();
    startAkcji=source->getStartAkcji();
    return true;
}