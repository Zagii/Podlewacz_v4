
#ifndef CONFIG_FS_H
#define CONFIG_FS_H

#include <LittleFS.h>

class ConfigFS
{
        char filename[16];
     //   bool isConfigChanged = false;
       // char path[20];
    public:
        ConfigFS(const char* file)
        {
            strcpy(filename,file);
           // strcpy(path,pathToFile);
        };
        void begin()
        {
            //check if file exist
            // N: create empty file
            // Y: skip, do nothing
        }
        String readFromFile()
        {
            Serial.print("config readFromFile: ");Serial.println(filename);
            if(!LittleFS.exists(filename))
            {
                Serial.print("-- brak pliku, tworze pusty.. ");
                File f=LittleFS.open(filename,"w");
                if(f)
                {
                    f.println("{}");
                    Serial.println("ok");
                }else Serial.println("err");

                f.close();
            }
            File file =LittleFS.open(filename, "r");
            String p="{}";
            if (file) 
            {
                p="";
                while(file.available())
                {
                 p+=file.readString();
                }
                file.close();
                
            }
            else
            {
                Serial.println( String(filename) + String(" open error"));
            }
            Serial.print("zawartosc pliku: ");Serial.println(p);
            return p;
        };
        String loadFromFile()
        {
            String p= readFromFile();
            if( parseFile(p) )
            {
                return p;
            }
            return String("parse failed");
        };
        bool saveToFile()
        {
            if(checkIfSave())
            {
                Serial.print("Saving config: "); Serial.println(filename);
                String s= prepareFile();
                Serial.println(s);
                File f=LittleFS.open(filename,"w");
                if(f)
                {
                    f.print(s.c_str());
                    Serial.println("zapis ok");
                }else Serial.println("blad zapisu");

                
                f.close();
            }
            return true;
        };

       bool checkIfSave()
        {
            Serial.print("compare config file: "); Serial.println(filename);
           String s1= readFromFile();
         //  Serial.print("w pliku: ");Serial.println(s1);
           String s2= prepareFile();
          // Serial.print(" konfig: ");Serial.println(s2);
           if( s1.equals(s2) )
           {
            Serial.println(".. not changed");
            return false;
           }else
           {
            Serial.println(".. changed");
            return true;
           }
            return false;
        }
        virtual bool parseFile(String json)=0;
        virtual String prepareFile()=0;

};

#endif