#include "configuration.h"

#ifdef ENABLE_SCD30

#include "Co2SensorSCD30.h"
#include <SparkFun_SCD30_Arduino_Library.h>

namespace
{
    SCD30 &getSensor()
    {
        static SCD30 theSensor{};
        return theSensor;
    }
} 

void Co2SensorSCD30::begin()
{
    if (getSensor().begin() == false)
    {
        Serial.println("Air sensor not detected. Please check wiring. Freezing...");
        while (1)
            ;
    }
}

uint16_t Co2SensorSCD30::getCo2()
{
    if(getSensor().dataAvailable())
        _co2ppm = getSensor().getCO2();
    return _co2ppm;
}

bool Co2SensorSCD30::isReady()
{
    if (_isReady)
        return true;
    _isReady = getSensor().dataAvailable();
    return _isReady;
}

#endif