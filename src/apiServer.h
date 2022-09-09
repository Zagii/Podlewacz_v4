#ifndef API_SERVER_H
#define API_SERVER_H
#include <Arduino.h>
#include <ESP8266WiFi.h>          // Replace with WiFi.h for ESP32
#include <ESP8266WebServer.h>     // Replace with WebServer.h for ESP32

#include "config/config.h"

#define API_TYP_CONF 0
#define API_TYP_CONF_SYSTEM 1
#define API_TYP_CONF_SEKCJE 2
#define API_TYP_CONF_PROGRAMY 3
#define API_TYP_CONF_SEKWENCJE 4

#define API_TYPE_JSON "text/json"
#define API_PARAM_NAME "plain"

class ApiServer
{
    ESP8266WebServer *server;
    Config *config;
        void restGetConf(uint8_t typConf);
        void restServerRouting(); 
        void restSetNtpConf();
        void restSetSekcjeConf();
        void restSetSekcjeStan();
        void restSetProgram();
        void restSetSekwencja();

        void restChangeProgram();

        void restDelProgram();

        void rootPage();
        void getHelloWord();
        bool testArgs();

        void returnError(String funkcja);
    public:
        ApiServer(){ };
        void begin(ESP8266WebServer *_server,Config* _conf);
        
        
        String printRequestToString();
        void getApi();
        void notFoundPage();
        bool handleFileRead(String path);
        String getContentType(String filename);
        int sendPostRequest(String url, String requestData);
};

#endif