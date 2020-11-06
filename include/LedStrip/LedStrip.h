#pragma once

#include <inttypes.h>
#include "utils.h"

#ifdef ENABLE_LEDSTRIP
#include <Adafruit_NeoPixel.h>
#endif

static class LedStrip
{
public:
    enum state
    {
        heating,
        measuring
    };
    LedStrip();
    void begin();
    void setPpm(uint16_t ppm);

    // make sure this method gets called every 20ms.
    // here we can implement fancy stuff like animations
    void go(state s = measuring);

private:
#ifdef ENABLE_LEDSTRIP
    Adafruit_NeoPixel _strip;

    uint16_t _ppm{};
    uint32_t _color{};
    bool _blinkState{true};
    CheckTimeSpanPassed _every500Millis;

    uint8_t _blueish = 0;
    bool direction = true;

    uint32_t translate(uint16_t ppm);
#endif
} ledStrip;