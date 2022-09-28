#include "config.h"


void Config::begin(Czas* _czas, HardwareOutput* _hw)
{
    czas=_czas;
    hw=_hw;
    sysConf.begin(czas);
    sekcjeConf.begin(hw);
    programConf.begin();
};

void Config::loop()
{
    sekcjeConf.loop();
    programConf.loop();
}
