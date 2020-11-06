#include "configuration.h"

#ifdef ENABLE_SCD30

#include "CO2Sensor/Co2SensorSCD30.h"
#include <SparkFun_SCD30_Arduino_Library.h>
#include "utils.h"

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
    if (!getSensor().begin())
    {
        Serial.println("Air sensor not detected. Please check wiring. Freezing...");
        Serial.flush();
        abort();
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
    static auto wait10Seconds = CheckTimeSpanPassed(10000);
    if (_isReady)
        return true;
    _isReady = wait10Seconds() && getSensor().dataAvailable();
    return _isReady;
}

#endif