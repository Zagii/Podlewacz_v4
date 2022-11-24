#ifndef CONFIG_H
#define CONFIG_H

#include "czas.h"
#include "hardwareOutput.h"
#include "configFileSystem.h"
#include "configFileSekcje.h"
#include "configFileProgram.h"



class Config
{
    
    HardwareOutput *hw;
    public:
    Czas* czas;
      ConfigFileSystem sysConf{"/sys.json"};
      ConfigFileSekcje sekcjeConf{"/sekcje.json"};
      ConfigFileProgram programConf{"/programy.csv","/sekwencje.csv"};
      Config(){};
      void begin(Czas* _czas,HardwareOutput* _hw);
      void loop();
      
      
};

#endif