#ifdef ENABLE_SCD30

#pragma once
#include "Co2Sensor.h"

static class Co2SensorSCD30 : public Co2Sensor
{
public:
    Co2SensorSCD30() = default;
    virtual void begin() override;
    virtual uint16_t getCo2() override;
    virtual bool isReady() override;

private:
    bool _isReady{};
    uint16_t _co2ppm{};
} co2Sensor;

#endif