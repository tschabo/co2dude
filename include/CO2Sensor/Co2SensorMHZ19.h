#ifdef ENABLE_MHZ19

#pragma once
#include "ICo2Sensor.h"
#include <MHZ19.h>
#include <SoftwareSerial.h>

static class Co2SensorMHZ19 : public ICo2Sensor
{
public:
    Co2SensorMHZ19() : _serial(CO2_SERIAL_RX_PIN, CO2_SERIAL_TX_PIN){}
    virtual void begin() override;
    virtual uint16_t getCo2() override;
    virtual bool isReady() override;

private:
    MHZ19 _mhz19;
    SoftwareSerial _serial;
} co2Sensor;

#endif