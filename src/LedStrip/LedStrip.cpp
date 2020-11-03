#include "LedStrip/LedStrip.h"
#include "configuration.h"
#include <Adafruit_NeoPixel.h>
#include "utils.h"

#ifndef ENABLE_LEDSTRIP
void LedStrip::begin(){};
void LedStrip::setPpm(uint16_t){};
void LedStrip::go(state){};
#else

namespace
{
    static Adafruit_NeoPixel &getStrip()
    {
        static auto strip(Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800));
        return strip;
    }
} // namespace

LedStrip::LedStrip() : _every500Millis(500) {}

void LedStrip::begin()
{
    getStrip().begin();
    getStrip().show();
    getStrip().setBrightness(50);
}

void LedStrip::setPpm(uint16_t ppm)
{
    _ppm = ppm;
}

uint32_t LedStrip::translate(uint16_t ppm)
{
    if (ppm < 700u)
    {
        if (ppm <= 400u)
            ppm = 400u;
        return Adafruit_NeoPixel::Color(0, 0xFF, 0);
    }
    else if (ppm < 1000u)
    {
        return Adafruit_NeoPixel::Color(0xFF, 0xFF, 0);
    }
    else if (ppm < 2000u)
    {
        return Adafruit_NeoPixel::Color(0xFF, 0, 0);
    }
    // everything else is >= 2000 ppm
    if (_every500Millis())
        _blinkState = !_blinkState;
    if (_blinkState)
        return Adafruit_NeoPixel::Color(0xFF, 0, 0);
    return Adafruit_NeoPixel::Color(0, 0, 0);
}

void LedStrip::go(state s)
{
    bool needsUpdate{false};
    switch (s)
    {
    case measuring:
    {
        auto currentPPM = translate(_ppm);
        if (currentPPM != _color)
        {
            needsUpdate = true;
            _color = currentPPM;
        }
        break;
    }
    case LedStrip::heating:
        static CheckTimeSpanPassed every500Millis(500);
        static uint8_t col = 0;
        static bool direction = true;
        if (every500Millis())
        {
            needsUpdate = true;
            if (col == 150 || col == 0)
                direction = !direction;
            _color = Adafruit_NeoPixel::Color(0, 0, col);
            direction ? col-- : col++;
            break;
        }
    }

    if (needsUpdate)
    {
        for (size_t i = 0; i < getStrip().numPixels(); i++)
        {
            getStrip().setPixelColor(i, _color);
        }
        getStrip().show();
    }
}

#endif