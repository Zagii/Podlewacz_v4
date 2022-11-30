 #include "sekwencja.h"

String Sekwencja::getSekwencjaCSVString()
{
    String ret=String(sekwencjaId)+String("|")+String(sekcjaId)+String("|");
    ret+=String(akcja?1:0)+ String("|")+String(czasTrwaniaAkcji)+String("|")+String(startAkcji)+String("|")+String(programId);
    return ret;
};
int Sekwencja::setSekwencjaFromCSV(String csv)
{
    Serial.println(__PRETTY_FUNCTION__);
    String pole="";
    int nrPola = 0;
    while (csv.length() > 0)
    {
        int index = csv.indexOf('|');
        if (index == -1) // programId
        {
            
            programId=(uint8_t) csv.toInt();
           // Serial.printf("programId: %d, str:%s, toInt: %d\n",programId,csv.c_str(),csv.toInt());
            break;
        }
        else
        {
            // 10|5|1|2|28|3
            //{"programId":3,"sekwencjaId":10,"sekcjaId":"5","akcja":1,"czasTrwaniaAkcji":2,"startAkcji":28}
            pole = csv.substring(0, index);
           // Serial.printf("nrPola: %d, pole %s\n",nrPola,pole.c_str());
            switch (nrPola)
            {
            case 0: //sekwencjaId
                sekwencjaId =(uint8_t)  pole.toInt();
                break;
            case 1: // sekcjaId
                sekcjaId =(uint8_t)  pole.toInt();
                break;
            case 2: // akcja
                akcja=pole.charAt(0)=='1'? true:false;
                break;
            case 3: // czasTrwaniaAkcji
                czasTrwaniaAkcji = pole.toInt();
                break;
            case 4: // startAkcji
                startAkcji=pole.toInt();
                break;
            case 5:
               // to juz nie jest wolane bo nie ma pipe na koncu
               // programId=(uint8_t) pole.toInt();
                break;
            default:
                //programId=pole.toInt();
                Serial.print(F("Zla liczba pol, "));Serial.println(pole);
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
        if(_id != ID_SEKWENCJI_NIEZNANE)
        {
            sekwencjaId=_id;
        }
        else{
            sekwencjaId = doc["sekwencjaId"] | _id;
        }
     //   if(sekwencjaId)
        programId=doc["programId"] | 254;
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
    String ret=String("{\"programId\":")+String(programId)+String(",\"sekwencjaId\":")+String(sekwencjaId)+
            String(",\"sekcjaId\":")+String(sekcjaId)+String(",\"akcja\":");
    ret+=String(akcja ? "1" : "0");
    ret+=String(",\"czasTrwaniaAkcji\":")+String(czasTrwaniaAkcji)+String(",\"startAkcji\":")+String(startAkcji);
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
    programId=source->getProgramId();
    return true;
}

///////////////////// nie uzywan
/*
uint8_t Sekwencja::testSekwencja(unsigned long aktualnaSekundaProgramu,uint8_t testSekcjaId, bool testAkcja)
{
    if(testSekcjaId!=sekcjaId)
    {
        return -1; // nie ta sekcja
    }
    //przedzial czasowy
    if(aktualnaSekundaProgramu>=startAkcji && aktualneaSekundaProgramu<startAkcji+czasTrwaniaAkcji)
    {
        if(testAkcja==akcja)
        {
            return 0; // sekcja juz jest w odpowiednim stanie
        }
    }else
    {
        return -2; // nie ten czas
    }
    return 1; //sekwencja sugeruje zmiane stanu
}*/