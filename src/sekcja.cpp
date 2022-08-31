#include "sekcja.h"

  void Sekcja::begin(HardwareOutput *h,uint8_t _id, String jsonConfig)
    {
        hw=h;
        parseConfig(_id,jsonConfig);
        init();
    };
    void Sekcja::begin(HardwareOutput *h,uint8_t _id, String _nazwa,int _typ, int _pin,bool _inverted,
        String _apiOnUrl, String _apiOnJson, String _apiOffUrl, String _apiOffJson)
    {
        hw=h;
        id=_id;
        nazwa=_nazwa; typ=_typ; pin=_pin; inverted=_inverted; 
        apiOnUrl=_apiOnUrl; apiOnJson=_apiOnJson;apiOffJson=_apiOffJson; apiOffUrl=_apiOffUrl;
        init();
    };
    void Sekcja::setState(bool state)
    {
        if(state==stan)return;
        Serial.print("setState sekcja: ");Serial.print(nazwa);Serial.print(", zmiana: ");
        Serial.print(stan);Serial.print("->");Serial.println(state);
        stan=state;
        switch(typ)
        {
            case SEKCJA_TYP_FIZYCZNA:
                    hw->setValue(pin,stan ? onStan : offStan );
                break;
            case SEKCJA_TYP_REST:
                int retCode=0;
                //call rest api
                if(stan==onStan)
                {
                    retCode=sendPostRequest(apiOnUrl,apiOnJson);
                }else
                {
                    retCode=sendPostRequest(apiOffUrl,apiOffJson);
                }
                Serial.print("post retCode: ");Serial.println(retCode);
                //todo ponawianie po nie udanym retCode (jakos w loop asynchronicznie najlepiej)
            break;
            //todo case SEKCJA_TYP_MQTT:
            //break;

        }
        lastStateChanged=millis();
    };
int Sekcja::sendPostRequest(String url, String requestData)
{
         Serial.print("sendPostRequest: ");Serial.print(url);Serial.print(", req: ");Serial.println(requestData);
         if(WiFi.status()== WL_CONNECTED)
         {
            WiFiClient client;
            HTTPClient http;
            
            // Your Domain name with URL path or IP address with path
            http.begin(client, url);

            // Specify content-type header
            //http.addHeader("Content-Type", "application/x-www-form-urlencoded");
            // Data to send with HTTP POST
            //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
            // Send HTTP POST request
            
            // If you need an HTTP request with a content type: application/json, use the following:
            http.addHeader("Content-Type", "application/json");
            //int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");

            // If you need an HTTP request with a content type: text/plain
            //http.addHeader("Content-Type", "text/plain");
            //int httpResponseCode = http.POST("Hello, World!");
            
            int httpResponseCode = http.POST(requestData);
            
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
                
            // Free resources
            http.end();
            return httpResponseCode;
            }
            else {
               Serial.println("WiFi Disconnected");
               return -1;
            }
            return -1;
};
void Sekcja::parseConfig(uint8_t _id, String jsonConfig)
     {
         Serial.println("Parsowanie konfiguracji sekcji ");
         Serial.println(jsonConfig);
         StaticJsonDocument<JSON_SIZE> doc;
         DeserializationError error = deserializeJson(doc, jsonConfig);
         if (error)
         {
             Serial.println("JSON de-serialization failed: " + String(error.c_str()));
             id=_id;
             nazwa = "Sekcja";
             typ = SEKCJA_TYP_FIZYCZNA;
             pin = 0;
             inverted = false;
             apiOnUrl = "";
             apiOnJson = "";
             apiOffJson = "";
             apiOffUrl = "";
             return;
         }
         else
         {
             id=doc["id"] | _id;
             nazwa = doc["nazwa"] | String("Sekcja");
             typ = doc["typ"] | SEKCJA_TYP_FIZYCZNA; // fizyczna, wirtualnaREST, wirtualnaMQTT
             pin = doc["pin"] | 0;                   // pin ktorym steruje
             inverted = doc["inverted"] | false;     // odwrocona logika na pinie
             apiOnUrl = doc["apiOnUrl"] | String("");
             apiOnJson = doc["apiOnJson"] | String("");
             apiOffUrl = doc["apiOffUrl"] | String("");
             apiOffJson = doc["apiOffJson"] | String("");
         }
    };