#include "apiServer.h"


void ApiServer::begin(ESP8266WebServer *_server, Config* _conf, Manager* _manager)
{
            server=_server;
            config=_conf;
            manager=_manager;
            server->enableCORS(true);
            restServerRouting();          
};
void ApiServer::restGetConf(uint8_t typConf)
{
    String resp="nieznane rządanie";
    int retCode=400;
    String contentType="text/html";
    switch(typConf)
    {
        case API_TYP_CONF:
            getApi();
            return;
        case API_TYP_CONF_SYSTEM: 
            retCode=200;
            contentType=API_TYPE_JSON;
            resp=config->sysConf.prepareFile();
        break;
        case API_TYP_CONF_SEKCJE: 
            retCode=200;
            contentType=API_TYPE_JSON;
            resp=config->sekcjeConf.getSekcjeJsonString();
        break;
        case API_TYP_CONF_PROGRAMY: 
            retCode=200;
            contentType=API_TYPE_JSON;
            resp=config->programConf.getProgramyJsonString(true);
        break;
        case API_TYP_CONF_SEKWENCJE: 
            retCode=200;
            contentType=API_TYPE_JSON;
            resp=config->programConf.getSekwencjeJsonString(true);
        break;
    };
    addCORS();
    Serial.println(resp);
    server->send(retCode, contentType, resp);   
}
bool ApiServer::testArgs()
{
    Serial.println(printRequestToString());
    if (server->hasArg(API_PARAM_NAME)== false){ //Check if body received
 
            returnError(__FUNCTION__);
            return false;
 
    }
    
    String message = "mam body, params:\n";
             message += server->arg(API_PARAM_NAME);
             message += "\n";
 
     
    Serial.println(message);
     return true;
}
void ApiServer::addCORS()
{
 //   server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
        server->sendHeader(F("Access-Control-Max-Age"), F("10000"));
        server->sendHeader(F("Access-Control-Allow-Methods"), F("PUT,POST,GET,DELETE,OPTIONS"));
        server->sendHeader(F("Access-Control-Allow-Headers"), F("*"));
}
void ApiServer::restSetNtpConf()
{
    addCORS();
    if(!testArgs()) return;
      
      StaticJsonDocument<JSON_SIZE> doc;
      DeserializationError error = deserializeJson(doc, server->arg(API_PARAM_NAME));
      if (error)
      {
                     Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
                     server->send(400, "text/plain","JSON de-serialization failed: " + String(error.c_str()));
                     return;
      }else
      {
            String ntpHost=doc["ntpHost"] | config->sysConf.ntpHost;
            long offset=doc["ntpOffset"] | config->sysConf.ntpOffset;
            uint8_t useNTP = doc["useNTP"] | config->sysConf.useNtp;
      /*      for (uint8_t i = 0; i < server->args(); i++) {
               // message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
                if(server->argName(i).compareTo("ntpHost")==0)   ntpHost=server->arg(i);
                if(server->argName(i).compareTo("ntpOffset")==0)   offset=server->arg(i).toInt();
                if(server->argName(i).compareTo("useNtp")==0)   useNTP=server->arg(i).toInt();
            }*/

            config->sysConf.setNtpParams(useNTP,ntpHost.c_str(),offset);
            server->send(200, API_TYPE_JSON, config->sysConf.prepareFile());
      }
};
void ApiServer::returnError(String funkcja)
{
        Serial.println(__FUNCTION__);
        Serial.println(funkcja);

        char errStr[70];
        strcpy(errStr,"{\"err\":\"Error ");
     //    Serial.println(errStr);
        strncat(errStr,funkcja.c_str(),65);
       //  Serial.println(errStr);
        strcat(errStr,"\"}");
       // Serial.println(strlen(errStr));
        Serial.println(errStr);
        server->send(500, API_TYPE_JSON, errStr);
}
/*
void ApiServer::restSetSekcjeConf()
{
    if(!testArgs()) return;
    if(config->sekcjeConf.setSekcjeConfig( server->arg(API_PARAM_NAME)))
    {
        server->send(200, API_TYPE_JSON, config->sekcjeConf.prepareFile());
    }else
    {
        returnError(__FUNCTION__);
    }
}*/
void ApiServer::restSetSekcjaConf(uint8_t reqType)
{
    Serial.println(__FUNCTION__);
        
    addCORS();
    if(!testArgs()) return;
    bool r;
/*    switch(reqType)
    {
        case HTTP_POST:
            r=config->sekcjeConf.addSekcjaAndSaveFile(server->arg(API_PARAM_NAME));
            break;
        case HTTP_PUT:
            r=config->sekcjeConf.changeSekcjaFromJsonStringAndSaveFile(server->arg(API_PARAM_NAME));
            break;
        default:
            r=false;
            break;
    }*/
    r=config->sekcjeConf.addChangeSekcjaAndSaveFile(server->arg(API_PARAM_NAME));
    if(r)
    {
        server->send(200, API_TYPE_JSON, config->sekcjeConf.getSekcjeJsonString());
    }else
    {
         returnError(__FUNCTION__);
    }
}
void ApiServer::restSetSekcjeStan()
{
    addCORS();
    if(!testArgs()) return;
    if(config->sekcjeConf.setSekcjeStan(server->arg(API_PARAM_NAME)))
    {
        server->send(200, API_TYPE_JSON, config->sekcjeConf.getSekcjeStan());
    }else
    {
       // server->send(500, "text/plain", "Error setSekcjeStan");
        returnError(__FUNCTION__);
    }
}
void ApiServer::restGetSekcjeStan()
{
    addCORS();
    if(!testArgs()) return;
    server->send(200, API_TYPE_JSON, config->sekcjeConf.getSekcjeStan());
    
}
void ApiServer::restSetProgram(uint8_t reqType)
{
    addCORS();
    if(!testArgs()) return;
    bool r;
  /*  switch(reqType)
    {
        case HTTP_POST:
            r=config->programConf.addProgramAndSaveFile(server->arg(API_PARAM_NAME));
            break;
        case HTTP_PUT:
            r=config->programConf.changeProgramFromJsonStringAndSaveFile(server->arg(API_PARAM_NAME));
            break;
        default:
            r=false;
            break;
    }*/
    r=config->programConf.addChangeProgramAndSaveFile(server->arg(API_PARAM_NAME));
    if(r)
    {
        server->send(200, API_TYPE_JSON, config->programConf.getProgramyJsonString(true));
    }else
    {
        //server->send(500, "text/plain", "Error setProgram");
         returnError(__FUNCTION__);
    }

}
void ApiServer::restSetSekwencja(uint8_t reqType)
{
    addCORS();
    if(!testArgs()) return;
    bool r;
    switch(reqType)
    {
        case HTTP_POST:
            r=config->programConf.addSekwencjaAndSaveFile(server->arg(API_PARAM_NAME));
            break;
        case HTTP_PUT:
            r=config->programConf.changeSekwencjaFromJsonStringAndSaveFile(server->arg(API_PARAM_NAME));
            break;
        default:
            r=false;
            break;
    }
    if(r)
    {
        server->send(200, API_TYPE_JSON, config->programConf.getSekwencjeJsonString(true));
    }else
    {
        //server->send(500, "text/plain", "Error setProgram");
         returnError(__FUNCTION__);
    }
}
/*
void ApiServer::restChangeProgram()
{
    if(!testArgs()) return;
    if(config->programConf.changeProgramFromJsonStringAndSaveFile(server->arg(API_PARAM_NAME)))
    {
        server->send(200, API_TYPE_JSON, config->programConf.getProgramyJsonString(true));
    }else
    {
        //server->send(500, "text/plain", "Error setProgram");
         returnError(__FUNCTION__);
    }

}*/
void ApiServer::restDelSekwencja()
{
    addCORS();
    if(!testArgs()) return;
    if(config->programConf.delSekwencjaFromJsonString(server->arg(API_PARAM_NAME)))
    {
        server->send(200, API_TYPE_JSON, config->programConf.getSekwencjeJsonString(true));
    }else
    {
        Serial.println("blad");
        //server->send(500, "text/plain", "Error setProgram");
         returnError(__FUNCTION__);
    }

}
void ApiServer::restDelProgram()
{
    addCORS();
    if(!testArgs()) return;
    if(config->programConf.delProgramFromJsonString(server->arg(API_PARAM_NAME)))
    {
        server->send(200, API_TYPE_JSON, config->programConf.getProgramyJsonString(true));
    }else
    {
        Serial.println("blad");
        //server->send(500, "text/plain", "Error setProgram");
         returnError(__FUNCTION__);
    }
}
void ApiServer::restDelSekcja()
{
    addCORS();
    if(!testArgs()) return;
    if(config->sekcjeConf.delSekcjaFromJsonString(server->arg(API_PARAM_NAME)))
    {
        server->send(200, API_TYPE_JSON, config->sekcjeConf.getSekcjeJsonString());
    }else
    {
        Serial.println("blad");
        //server->send(500, "text/plain", "Error setProgram");
         returnError(__FUNCTION__);
    }
}
void ApiServer::restStartProgram()
{
    addCORS();
    Serial.println(__PRETTY_FUNCTION__);
    if(!testArgs()) return;
    StaticJsonDocument<JSON_SIZE> doc;
    DeserializationError error = deserializeJson(doc, server->arg(API_PARAM_NAME));
    if (error)
    {
                     Serial.println( "JSON de-serialization failed: " + String(error.c_str()));
                     server->send(400, "text/plain","JSON de-serialization failed: " + String(error.c_str()));
                     return;
    }else
    {
        uint8_t programId=doc["programId"] | ID_PROGRAMU_NIEZNANE;
        if(manager->startProgram(programId))
        {
            server->send(200, API_TYPE_JSON, manager->getStatusJson());
        }else
        {
            Serial.println("blad");
            //server->send(500, "text/plain", "Error setProgram");
            returnError(__FUNCTION__);
        }
    }
};
void ApiServer::restStopProgram()
{
    addCORS();
    Serial.println(__PRETTY_FUNCTION__);
    if(manager->stopProgram())
    {
        server->send(200, API_TYPE_JSON, manager->getStatusJson());
    }else
    {
        Serial.println("blad");
            //server->send(500, "text/plain", "Error setProgram");
        returnError(__FUNCTION__);
    }
};
void ApiServer::restServerRouting() 
{
          //  server->on("/", HTTP_GET, []() {
 //                   server->send(200, F("text/html"),F("Welcome to the REST Web Server"));
   //         });
        //    server->on("/", HTTP_GET,[this](){handleFileRead("/index.html");});


            
           /* server->on("/assets/config.json", HTTP_GET,[this](){
                testArgs();//CORS headers
                char c[50];
                sprintf(c,"{\"ipUrl\":\"http://%d.%d.%d.%d\"}",WiFi.localIP()[0],WiFi.localIP()[1],WiFi.localIP()[2],WiFi.localIP()[3]);
                Serial.println(c);
                server->send(200, API_TYPE_JSON, c);
            });*/
            /**** get *****/
            server->on("/api/get", HTTP_GET,[this](){restGetConf(API_TYP_CONF);});
            server->on("/api/get/stan", HTTP_GET,[this](){restGetSekcjeStan();});
            server->on("/api/get/system", HTTP_GET,[this](){restGetConf(API_TYP_CONF_SYSTEM);});
            server->on("/api/get/sekcje", HTTP_GET,[this](){restGetConf(API_TYP_CONF_SEKCJE);});
            server->on("/api/get/programy", HTTP_GET,[this](){restGetConf(API_TYP_CONF_PROGRAMY);});
            server->on("/api/get/sekwencje", HTTP_GET,[this](){restGetConf(API_TYP_CONF_SEKWENCJE);});
            
            /****** set ****/
            server->on("/api/set/ntp", HTTP_POST,[this](){restSetNtpConf();});
          //  server->on("/api/set/sekcje", HTTP_POST,[this](){restSetSekcjeConf();});
            server->on("/api/set/sekcja", HTTP_POST,[this](){restSetSekcjaConf(HTTP_POST);});
            server->on("/api/set/stan", HTTP_POST,[this](){restSetSekcjeStan();});
            server->on("/api/set/program", HTTP_POST,[this](){restSetProgram(HTTP_POST);});
            server->on("/api/set/sekwencja", HTTP_POST,[this](){restSetSekwencja(HTTP_POST);});

            /********* change *********/
            server->on("/api/set/program", HTTP_PUT,[this](){restSetProgram(HTTP_PUT);});
            server->on("/api/set/sekcja", HTTP_PUT,[this](){restSetSekcjaConf(HTTP_PUT);});
            server->on("/api/set/sekwencja", HTTP_PUT,[this](){restSetSekwencja(HTTP_PUT);});

            /****** delete *****/
            server->on("/api/del/program", HTTP_DELETE, [this](){restDelProgram();});
            server->on("/api/del/sekwencja", HTTP_DELETE, [this](){restDelSekwencja();});
            server->on("/api/del/sekcja", HTTP_DELETE, [this](){restDelSekcja();});

            /*** inne ***/
            server->on("/api/start/program", HTTP_POST,[this](){restStartProgram();});
            server->on("/api/stop/program", HTTP_POST,[this](){restStopProgram();});


            server->on("/root", [this](){rootPage();});
            server->on(F("/api"), HTTP_GET, [this]{getApi();});
            server->on(F("/hello"), HTTP_GET, [this] {getHelloWord();});
        
          //  ac.getAutoConnect()->onNotFound([this] {notFoundPage();});
            server->onNotFound([this]{notFoundPage();});
       
};

void ApiServer::rootPage()
{
            Serial.println("rootPage");
             char content[] = "Hello, world";
            server->send(200, "text/plain", content);

};
void ApiServer::getHelloWord() 
{
            server->send(200, API_TYPE_JSON, "{\"name\": \"Hello world\"}");
    
};
String ApiServer::printRequestToString()
{
            String message = "API\n\n";
            message += "URI: ";
            message += server->uri();
            message += "\nMethod: ";
            message += (server->method() == HTTP_GET) ? "GET" : "POST";
            message += "\nArguments: ";
            message += server->args();
            message += "\n";
            for (uint8_t i = 0; i < server->args(); i++) {
                message += String(i) + String("| ") + " " + server->argName(i) + ": " + server->arg(i) + "\n";
            }
            return message;
};
void ApiServer::getApi() 
{
            Serial.print(F("getApi: "));
            String message = printRequestToString();

            message+="/api/get/sys - konfiguracja systemu \n";
            message+="/api/get/sekcje - konfiguracja sekcji\n";
            message+="/api/get/prog - konfiguracja programow\n";
            //http://192.168.1.178/set/ntp?useNtp=1&ntpHost=pool.ntp.org&ntpOffset=7200
            server->send(404, "text/plain", message);
            Serial.println(message);
};
void ApiServer::notFoundPage()
{
            Serial.println("notFoundPage");
            if(server->method()==HTTP_OPTIONS)
            {
                addCORS();
                server->send(200);
                return;
            }
        //             char content[] = "not found";
        //          server->send(404, "text/plain", content);
            if (!handleFileRead(server->uri()))                  // send it if it exists
            {
                server->send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
            }

};
bool ApiServer::handleFileRead(String path)
{  // send the right file to the client (if it exists)
            Serial.println("handleFileRead: " + path);
            Serial.print(F("methoda: "));Serial.println(server->method());
            if(path.endsWith("/")) path += "index.html";           // If a folder is requested, send the index file
            String jsString="";
            if(path.endsWith("ws.js"))
            {
                jsString="var wsSerw=\""+WiFi.localIP().toString()+"\";";
            }
            String contentType = getContentType(path);             // Get the MIME type
            String pathWithGz = path + ".gz";
            if(LittleFS.exists(pathWithGz) || LittleFS.exists(path)){  // If the file exists, either as a compressed archive, or normal
                if(LittleFS.exists(pathWithGz))                          // If there's a compressed version available
                path += ".gz";                                         // Use the compressed version
                File file = LittleFS.open(path, "r");                    // Open the file
                size_t sent=0;
                if(jsString!="")
                {
                // Serial.println("!!!!!!!!!!!!!!!  PRZED  !!!!!!!!!!!!!!!");
                // Serial.println(jsString);
                // Serial.println("!!!!!!!!!!!!!!!  PO  !!!!!!!!!!!!!!!");
                // jsString=file.readString();
                // Serial.println(jsString);
                server->send(200,contentType,jsString);
                }else
                {
                     sent = server->streamFile(file, contentType);    // Send it to the client
                     if(sent==0) Serial.println("streamFile, size=0");
                }
                file.close();                                          // Close the file again
                Serial.println(String("\tSent file: ") + path);
                return true;
            }
            Serial.println(String("\tFile Not Found: ") + path);
            return false;                                          // If the file doesn't exist, return false
};
String ApiServer::getContentType(String filename)
{
            if(server->hasArg("download")) return "application/octet-stream";
            else if(filename.endsWith(".htm")) return "text/html";
            else if(filename.endsWith(".html")) return "text/html";
            else if(filename.endsWith(".css")) return "text/css";
            else if(filename.endsWith(".js")) return "application/javascript";
            else if(filename.endsWith(".png")) return "image/png";
            else if(filename.endsWith(".gif")) return "image/gif";
            else if(filename.endsWith(".jpg")) return "image/jpeg";
            else if(filename.endsWith(".ico")) return "image/x-icon";
            else if(filename.endsWith(".xml")) return "text/xml";
            else if(filename.endsWith(".pdf")) return "application/x-pdf";
            else if(filename.endsWith(".zip")) return "application/x-zip";
            else if(filename.endsWith(".gz")) return "application/x-gzip";
            return "text/plain";
            // dodac Response.AppendHeader("Content-Encoding", "gzip"); ale nie wiem gdzie jeszcze
        // Content-Type: application/javascript
        // Content-Encoding: gzip
};
int ApiServer::sendPostRequest(String url, String requestData)
{
     /*    Serial.print("sendPostRequest: ");Serial.print(url);Serial.print(", req: ");Serial.println(requestData);
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
            return -1;*/
            return -1;
};