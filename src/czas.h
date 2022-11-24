#ifndef CZAS_H
#define CZAS_H

#include <NTPClient.h>
#include <WiFiUdp.h>
#include "RTClib.h"

#define RTC_INTERVAL_MS 400

class Czas
{
    NTPClient timeClient;
    RTC_DS1307 rtc;
    DateTime now;
    bool ntpConnected=false;
    bool useNTP=true;
    unsigned long rtcLastUpdate=0;
    public:
     Czas(WiFiUDP& ntpUDP):timeClient(ntpUDP){};
     void setNTP(bool isActive,const char* pool,long offset)
     {
        useNTP=isActive;
       
       
        timeClient.setPoolServerName(pool);
        timeClient.setTimeOffset(offset);
        
       
        ntpConnected=timeClient.update();
        
        Serial.print("NTP: ");
        if(ntpConnected)
        {
            Serial.println(getTimeStringNTP());
        }else
        {
            Serial.println("not ready");
        }

     };
     
     void begin(bool _useNtp,const char* pool,long offset)
     {
        Serial.print("Init Czas ");
        setNTP(_useNtp,pool,offset);
        timeClient.begin();
        if (! rtc.begin()) 
        {
            Serial.println("Couldn't find RTC");
        }

        if (! rtc.isrunning()) 
        {
              Serial.println("RTC is NOT running, let's set the time!");
            // When time needs to be set on a new device, or after a power loss, the
            // following line sets the RTC to the date & time this sketch was compiled
            rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
            // This line sets the RTC with an explicit date & time, for example to set
            // January 21, 2014 at 3am you would call:
            // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
        }
        now=rtc.now();
        Serial.println(getTimeStringRTC());
        loop();
     };
     void loop()
     {
         
         if(useNTP) 
         {
            ntpConnected=timeClient.update();
         }else
         {
            ntpConnected=false;
         }
         if(millis()-rtcLastUpdate>RTC_INTERVAL_MS)
         {
            now=rtc.now();
            rtcLastUpdate=millis();
         }
         if(ntpConnected) // w tym cyklu udalo sie odczytac czas ntp
         {
            if(abs(long(now.unixtime()-timeClient.getEpochTime()))>RTC_INTERVAL_MS*2)
            {
                rtc.adjust(DateTime(timeClient.getEpochTime()));
                now=rtc.now();
                rtcLastUpdate=millis();
                Serial.print("Aktualizacja RTC: ");Serial.println(getTimeStringRTC());
            }
         }
        //odczytaj czas z rtc
        //odczytaj czas z ntp
        //jesli dziala ntp jest net i jesli czasy sie bardzo roznia zaktualizuj rtc
     };
     String timeToString(unsigned long t)
     {
        DateTime dt=DateTime(t);
        String ret=String(dt.dayOfTheWeek())+"h"+String(dt.hour())+":"+String(dt.minute())+":"+String(dt.second());
        //char buf1[20];
        //DateTime now = rtc.now(); 
        //Updated now.day to now.date
        //sprintf(buf1, "%02d:%02d:%02d %02d/%02d/%02d",  now.hour(), now.minute(), now.second(), now.date(), now.month(), now.year()); 

        return ret;
     }
     String getDateTime()
     {
        String ret="";
        return ret;
     };
     String getTime()
     {
        String ret=timeClient.getFormattedTime();
        return ret;
     };
     public: unsigned long getTimeInSecondsNTP()
     {
         if(!useNTP) return 0;
        //zmienic na czas z zegarkow
        return timeClient.getEpochTime();
     };
     public: unsigned long getTimeInSecondsRTC()
     {
        //zmienic na czas z zegarkow
        return now.unixtime()+ ((millis() - rtcLastUpdate) / 1000);
     };
     String getTimeStringRTC()
     {
        return timeToString(getTimeInSecondsRTC());
     };
    String getTimeStringNTP()
    {
        return timeToString(getTimeInSecondsNTP());
    };
};

#endif