#ifndef CONFIG_H
#define CONFIG_H

#include "czas.h"
#include "hardwareOutput.h"
#include "configFileSystem.h"
#include "configFileSekcje.h"

class Config
{
    Czas* czas;
    HardwareOutput *hw;
    public:
      ConfigFileSystem sysConf{"/sys.json"};
      ConfigFileSekcje sekcjeConf{"/sekcje.json"};
      Config(){};
      void begin(Czas* _czas,HardwareOutput* _hw);
      void loop();
      
};

#endif