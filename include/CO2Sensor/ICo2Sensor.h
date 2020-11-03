#pragma once
#include <stdint.h>

class ICo2Sensor
{
public:
    virtual void begin() = 0;
    virtual uint16_t getCo2() = 0;
    virtual bool isReady() = 0;
};

#ifdef ENABLE_MHZ19
#include "Co2SensorMHZ19.h"
#elif defined(ENABLE_SCD30)
#include "Co2SensorSCD30.h"
#else
#error no sensor selected
#endif