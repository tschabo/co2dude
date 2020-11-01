#include "configuration.h"
#include "utils.h"

#ifdef ENABLE_MHZ19

#include "Co2SensorMHZ19.h"

void Co2SensorMHZ19::begin()
{
    _serial.begin(9600);
    _mhz19.begin(_serial);
}

uint16_t Co2SensorMHZ19::getCo2()
{
    return _mhz19.getCO2();
}

bool Co2SensorMHZ19::isReady()
{
    static CheckTimeSpanPassed heatingTimespanPassed(1000 * 60 * 3); // 3 minutes for heating according to datasheet
    static bool initialHeatingDone{false};
    
    if (!initialHeatingDone && heatingTimespanPassed())
        initialHeatingDone = true;
    else
        return false;

    static CheckTimeSpanPassed every100millis(500, true);
    static bool ready{false};

    if (every100millis())
        ready = _mhz19.getCO2() != 0;
    return ready;
}

#endif