#ifndef API_SERVER_H
#define API_SERVER_H

//extern ESP8266WebServer server;
//extern AutoConnectSetup ac;

class ApiServer
{
    ESP8266WebServer *server;
    Config *config;
    public:
        ApiServer()
        {
           
        };
        void begin(ESP8266WebServer *_server,Config* _conf)
        {
            server=_server;
            config=_conf;
            restServerRouting();          
        };
        void restGetConf()
        {
            server->send(200, F("text/html"),config->sysConf.prepareFile());   
        }
        void restServerRouting() 
        {
          //  server->on("/", HTTP_GET, []() {
 //                   server->send(200, F("text/html"),F("Welcome to the REST Web Server"));
   //         });
        //    server->on("/", HTTP_GET,[this](){handleFileRead("/index.html");});
            server->on("/get", HTTP_GET,[this](){restGetConf();});
            server->on("/set/ntp", HTTP_GET,[this](){restSetNtpConf();});
            server->on("/root", [this](){rootPage();});
            server->on(F("/api"), HTTP_GET, [this]{getApi();});
            server->on(F("/hello"), HTTP_GET, [this] {getHelloWord();});
          //  ac.getAutoConnect()->onNotFound([this] {notFoundPage();});
            server->onNotFound([this]{notFoundPage();});
       
        };
        void restSetNtpConf()
        {
            Serial.println(printRequestToString());
            String ntpHost=config->sysConf.ntpHost;
            long offset=config->sysConf.ntpOffset;
            uint8_t useNTP = config->sysConf.useNtp;
            for (uint8_t i = 0; i < server->args(); i++) {
               // message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
                if(server->argName(i).compareTo("ntpHost")==0)   ntpHost=server->arg(i);
                if(server->argName(i).compareTo("ntpOffset")==0)   offset=server->arg(i).toInt();
                if(server->argName(i).compareTo("useNtp")==0)   useNTP=server->arg(i).toInt();
            }
            config->sysConf.setNtpParams(useNTP,ntpHost.c_str(),offset);
        }
        void rootPage()
        {
            Serial.println("rootPage");
             char content[] = "Hello, world";
            server->send(200, "text/plain", content);

        };
        void getHelloWord() 
        {
            server->send(200, "text/json", "{\"name\": \"Hello world\"}");
    
        };
        String printRequestToString()
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
                message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
            }
            return message;
        }
        void getApi() 
        {
            Serial.print(F("getApi: "));
            String message = printRequestToString();

            message+="/api/get/sys - konfiguracja systemu \n";
            message+="/api/get/sekcje - konfiguracja sekcji\n";
            message+="/api/get/prog - konfiguracja programow\n";
            server->send(404, "text/plain", message);
            Serial.println(message);
        };
        void notFoundPage()
        {
            Serial.println("notFoundPage");
        //             char content[] = "not found";
        //          server->send(404, "text/plain", content);
            if (!handleFileRead(server->uri()))                  // send it if it exists
            {
                server->send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
            }

        };
        bool handleFileRead(String path)
        {  // send the right file to the client (if it exists)
            Serial.println("handleFileRead: " + path);
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
                size_t sent;
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
                }
                file.close();                                          // Close the file again
                Serial.println(String("\tSent file: ") + path);
                return true;
            }
            Serial.println(String("\tFile Not Found: ") + path);
            return false;                                          // If the file doesn't exist, return false
            };


        String getContentType(String filename){
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
       
};

#endif