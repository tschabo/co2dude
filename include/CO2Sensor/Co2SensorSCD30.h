#ifdef ENABLE_SCD30

#pragma once
#include "ICo2Sensor.h"

static class Co2SensorSCD30 : public ICo2Sensor
{
public:
    Co2SensorSCD30() = default;
    virtual void begin() override;
    virtual uint16_t getCo2() override;
    virtual bool isReady() override;
    virtual void calibrate() override;

private:
    bool _isReady{};
    uint16_t _co2ppm{};
} co2Sensor;

#endif