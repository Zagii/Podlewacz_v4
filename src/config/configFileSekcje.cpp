#include "configFileSekcje.h"

void ConfigFileSekcje::begin(HardwareOutput *_h/*, ApiServer *_api*/)
{
            hardwareOutput=_h;
      //      apiServer=_api;
            parseFile(readFromFile());
};
void ConfigFileSekcje::loop()
{
    for(int i=0;i<liczbaSekcji;i++)
    {
        sekcje[i]->loop();
    }
}
bool ConfigFileSekcje::parseFile(String json)
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
};

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

    
};

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