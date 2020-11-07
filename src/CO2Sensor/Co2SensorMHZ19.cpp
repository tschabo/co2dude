#include "configuration.h"
#include "utils.h"

#ifdef ENABLE_MHZ19

#include "CO2Sensor/Co2SensorMHZ19.h"

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
    static bool ready{false};

    if (ready)
        return true;

    static auto heatingTimespanPassed = CheckTimeSpanPassed(1000 * 60 * 3, false); // 3 minutes for heating according to datasheet
    static bool initialHeatingDone{false};

    if (!initialHeatingDone && heatingTimespanPassed())
        initialHeatingDone = true;
    else
        return false;

    static auto every100millis = CheckTimeSpanPassed(500, true);
    if (every100millis())
        ready = _mhz19.getCO2() != 0;
    return ready;
}

void Co2SensorMHZ19::calibrate()
{
    Serial.println("calibration is not implemented for MHZ19");
}

#endif