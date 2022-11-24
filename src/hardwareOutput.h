#ifndef HARDWAREOUTPUT_H
#define HARDWAREOUTPUT_H

#include <PCF8574.h>

#define MAX_OUT 8
#define UPDATE_INTERVAL_MS 300

class HardwareOutput
{

    PCF8574 pcf8574{0x38};
    byte outValueByte;
    PCF8574::DigitalInput outValue;
    unsigned long lastUpdateMillis;
    public:
    HardwareOutput(){};
    void begin()
    {
        for(int i=0;i<8;i++) {
            pcf8574.pinMode(i, OUTPUT);
            setValue(i,LOW);
        }
        Serial.print("Init pcf8574...");
        if (pcf8574.begin()){
            Serial.println("OK");
        }else{
            Serial.println("error");
        }
        pcf8574.digitalWriteAll(outValue);
      //  pcf8574.digitalWriteAllBytes(outValue);
        lastUpdateMillis=millis();
    };
    void setValue(uint8_t pin,uint8_t value)
    {
        Serial.println(__PRETTY_FUNCTION__);
        switch (pin)
        {
            case 0: outValue.p0=value;  break;
            case 1: outValue.p1=value;  break;
            case 2: outValue.p2=value;  break;
            case 3: outValue.p3=value;  break;
            case 4: outValue.p4=value;  break;
            case 5: outValue.p5=value;  break;
            case 6: outValue.p6=value;  break;
            case 7: outValue.p7=value;  break;
        
        }
        pcf8574.digitalWriteAll(outValue);
    }
 /*   void setValByte(uint8_t pin, uint8_t value)
    {
		if (value==HIGH){
			outValueByte = outValueByte | bit(pin);
			
		}else{
			outValueByte = outValueByte & ~bit(pin);		
		}

	}*/
    void loop()
    {
        //if(millis()-lastUpdateMillis<UPDATE_INTERVAL_MS)return;
        
        //pcf8574.digitalWriteAll(outValue);
        //lastUpdateMillis=millis();

        /* static int pin = 0;
        pcf8574.digitalWrite(pin, HIGH);
        delay(1000);
        pcf8574.digitalWrite(pin, LOW);
        delay(1000);
        pin++;
        if (pin > 7) pin = 0;*/
    };

};

#endif