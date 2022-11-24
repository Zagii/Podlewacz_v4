#include "sekcja.h"

void Sekcja::init()
{
    if (inverted)
    {
        onStan = LOW;
        offStan = HIGH;
    }
    autoSwitchActive = false;
    setOff();
};
int Sekcja::begin(HardwareOutput *h,  String jsonConfig)
{
    hw = h;
    //parseConfig(_id, jsonConfig);
    int r=setSekcjaFromJson(jsonConfig);
    init();
    return r;
};/*
void Sekcja::begin(HardwareOutput *h, uint8_t _id, String _nazwa, int _typ, int _pin, bool _inverted,
                   String _apiOnUrl, String _apiOnJson, String _apiOffUrl, String _apiOffJson)
{
    hw = h;
    id = _id;
    nazwa = _nazwa;
    typ = _typ;
    pin = _pin;
    inverted = _inverted;
    apiOnUrl = _apiOnUrl;
    apiOnJson = _apiOnJson;
    apiOffJson = _apiOffJson;
    apiOffUrl = _apiOffUrl;
    init();
};*/
void Sekcja::setState(bool state)
{
    if (state == stan)
        return;
    Serial.print("setState sekcja: ");
    Serial.print(nazwa);
    Serial.print(", zmiana: ");
    Serial.print(stan);
    Serial.print("->");
    Serial.println(state);
    stan = state;
    switch (typ)
    {
    case SEKCJA_TYP_FIZYCZNA:
        hw->setValue(pin, stan ? onStan : offStan);
        break;
    case SEKCJA_TYP_REST:
        int retCode = 0;
        // call rest api
        if (stan == onStan)
        {
            retCode = sendPostRequest(apiOnUrl, apiOnJson);
        }
        else
        {
            retCode = sendPostRequest(apiOffUrl, apiOffJson);
        }
        Serial.print("post retCode: ");
        Serial.println(retCode);
        // todo ponawianie po nie udanym retCode (jakos w loop asynchronicznie najlepiej)
        break;
        // todo case SEKCJA_TYP_MQTT:
        // break;
    }
    lastStateChanged = millis();
};
int Sekcja::sendPostRequest(String url, String requestData)
{
    Serial.print("sendPostRequest: ");
    Serial.print(url);
    Serial.print(", req: ");
    Serial.println(requestData);
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiClient client;
        HTTPClient http;

        // Your Domain name with URL path or IP address with path
        http.begin(client, url);

        // Specify content-type header
        // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        // Data to send with HTTP POST
        // String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";
        // Send HTTP POST request

        // If you need an HTTP request with a content type: application/json, use the following:
        http.addHeader("Content-Type", "application/json");
        // int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");

        // If you need an HTTP request with a content type: text/plain
        // http.addHeader("Content-Type", "text/plain");
        // int httpResponseCode = http.POST("Hello, World!");

        int httpResponseCode = http.POST(requestData);

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        // Free resources
        http.end();
        return httpResponseCode;
    }
    else
    {
        Serial.println("WiFi Disconnected");
        return -1;
    }
    return -1;
};/*
void Sekcja::parseConfig(uint8_t _id, String jsonConfig)
{
    bool czyEdycja = false;
    Serial.println("Parsowanie konfiguracji sekcji ");
    Serial.println(jsonConfig);
    StaticJsonDocument<JSON_SIZE> doc;
    DeserializationError error = deserializeJson(doc, jsonConfig);
    if (error)
    {
        Serial.println("JSON de-serialization failed: " + String(error.c_str()));
        id = _id;
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
        if (doc["id"])
        {
            id = doc["id"];
            czyEdycja = true;
            if (id < 0)
            {
                id = _id;
                czyEdycja = false;
            }
        }
        else
        {
            id = _id;
            czyEdycja = false;
        }

        nazwa = doc["nazwa"] | String("Sekcja");
        typ = doc["typ"] | SEKCJA_TYP_FIZYCZNA; // fizyczna, wirtualnaREST, wirtualnaMQTT
        pin = doc["pin"] | 0;                   // pin ktorym steruje
        inverted = doc["inverted"] | false;     // odwrocona logika na pinie
        apiOnUrl = doc["apiOnUrl"] | String("");
        apiOnJson = doc["apiOnJson"] | String("");
        apiOffUrl = doc["apiOffUrl"] | String("");
        apiOffJson = doc["apiOffJson"] | String("");
    }
    return; // czyEdycja;
};*/

/* **********************************************************************
************ parsuje stringa z konfiguracja sekcji na pola
** wejście: jsonConfig
** return: id sekcji odczytana z jsonConfig lub -1 jesli id nie podano
************************************************************************** */
int Sekcja::setSekcjaFromJson(String jsonConfig)
{
    Serial.println(__PRETTY_FUNCTION__);
    Serial.println(jsonConfig);
    StaticJsonDocument<JSON_SIZE> doc;
    DeserializationError error = deserializeJson(doc, jsonConfig);
    if (error)
    {
        Serial.println("JSON de-serialization failed: " + String(error.c_str()));
        id = SEKCJA_ID_BRAK;
        nazwa = "Sekcja";
        typ = SEKCJA_TYP_FIZYCZNA;
        pin = 0;
        inverted = false;
        apiOnUrl = "";
        apiOnJson = "";
        apiOffJson = "";
        apiOffUrl = "";
        return SEKCJA_ID_BLAD;
    }
    else
    {
        id = doc["sekcjaId"] | SEKCJA_ID_BRAK;
        nazwa = doc["nazwa"] | String("Sekcja");
        typ = doc["typ"] | SEKCJA_TYP_FIZYCZNA; // fizyczna, wirtualnaREST, wirtualnaMQTT
        pin = doc["pin"] | 0;                   // pin ktorym steruje
        inverted = doc["inverted"] | false;     // odwrocona logika na pinie
        apiOnUrl = doc["apiOnUrl"] | String("");
        apiOnJson = doc["apiOnJson"] | String("");
        apiOffUrl = doc["apiOffUrl"] | String("");
        apiOffJson = doc["apiOffJson"] | String("");
    }
    return id;
};
/* **********************************************************************
************ kopia danych z sekcji source do bieżącego obiektu
************************************************************************ */
bool Sekcja::copySekcja(Sekcja *source)
{
    id = source->id;
    nazwa = source->nazwa;
    typ = source->typ;
    pin = source->pin;
    inverted = source->inverted;
    apiOnUrl = source->apiOnUrl;
    apiOnJson = source->apiOnJson;
    apiOffUrl = source->apiOffUrl;
    apiOffJson = source->apiOffJson;
    return true;
}

String Sekcja::getSekcjaJsonString()
{
    String ret = "";

    ret = String("{\"sekcjaId\":") + String(id);

    ret += String(",\"nazwa\":\"") + String(nazwa) + String("\"");
    ret += String(",\"typ\":") + String(typ);

    if (typ == SEKCJA_TYP_FIZYCZNA)
    {
        ret += String(",\"pin\":") + String(pin);
    }

    ret += String(",\"inverted\":") + String(inverted?1:0);
    if (typ == SEKCJA_TYP_REST)
    {
        ret += String(",\"apiOnUrl\":\"") + String(apiOnUrl) + String("\"");
        ret += String(",\"apiOnJson\":\"") + String(apiOnJson) + String("\"");
        ret += String(",\"apiOffUrl\":\"") + String(apiOffUrl) + String("\"");
        ret += String(",\"apiOffJson\":\"") + String(apiOffJson) + String("\"");
    }

    ret += String("}");
    return ret;
}
void Sekcja::setState(uint8_t state)
{
    autoSwitchActive = false;
    setState(state > 0 ? true : false);
};
void Sekcja::setOn()
{
    autoSwitchActive = false;
    setState(true);
};
void Sekcja::setOff()
{
    autoSwitchActive = false;
    setState(false);
};
void Sekcja::setStateForTime(uint8_t state, long secondsToSwitch)
{
    setStateForTime(state > 0 ? true : false, secondsToSwitch);
}
void Sekcja::setStateForTime(bool state, long secondsToSwitch)
{
    Serial.printf("setStateForTime state: %d, for: %lu \n", state ? 1 : 0, secondsToSwitch);
    if (secondsToSwitch <= 0)
    {
        setState(state);
    }
    else
    {
        if (state)
            setOnForTime(secondsToSwitch);
        else
            setOffForTime(secondsToSwitch);
    }
}
void Sekcja::setOnForTime(long onForSeconds)
{
    autoSwitchActive = true;
    stateToSwitch = false;
    setState(true);
    timeToSwitch = onForSeconds * 1000;
};
void Sekcja::setOffForTime(long offForSeconds)
{
    autoSwitchActive = true;
    stateToSwitch = true;
    setState(false);
    timeToSwitch = offForSeconds * 1000;
};

void Sekcja::loop()
{
    if (autoSwitchActive)
    {
        if (millis() - lastStateChanged > timeToSwitch)
        {
            Serial.printf("autoSwitch, sekcjaId: %d, zmiana na: %d\n", id, stateToSwitch ? 1 : 0);
            autoSwitchActive = false;
            setState(stateToSwitch);
        }
    }
};

String Sekcja::getStanJson()
{
    String ret = "";
    ret = String("{\"sekcjaId\":") + String(id);
    ret += String(",\"stan\":") + String(stan ? 1 : 0);
    ret += String(",\"autoSwitchActive\":") + String(autoSwitchActive);
    if (autoSwitchActive)
    {
        ret += String(",\"timeToSwitch\":") + String(timeToSwitch);
        ret += String(",\"stateToSwitch\":") + String(stateToSwitch);
    }
    ret += String(",\"lastStateChanged\":") + String(lastStateChanged);
    ret += String("}");
    return ret;
};

/*
String Sekcja::prepareConfigJson()
{
    String ret = "";

    ret = String("{\"sekcjaId\":") + String(id);

    ret += String(",\"nazwa\":\"") + String(nazwa) + String("\"");
    ret += String(",\"typ\":") + String(typ);

    if (typ == SEKCJA_TYP_FIZYCZNA)
    {
        ret += String(",\"pin\":") + String(pin);
    }
    // todo adres hardware jesli wiecej sekcji niz 8
    ret += String(",\"inverted\":") + String(inverted);
    if (typ == SEKCJA_TYP_REST)
    {
        ret += String(",\"apiOnUrl\":\"") + String(apiOnUrl) + String("\"");
        ret += String(",\"apiOnJson\":\"") + String(apiOnJson) + String("\"");
        ret += String(",\"apiOffUrl\":\"") + String(apiOffUrl) + String("\"");
        ret += String(",\"apiOffJson\":\"") + String(apiOffJson) + String("\"");
    }

    ret += String("}");
    return ret;
};*/