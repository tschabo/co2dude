#include "LedStrip/LedStrip.h"
#include "configuration.h"

#ifndef ENABLE_LEDSTRIP
LedStrip::LedStrip(){}
void LedStrip::begin(){};
void LedStrip::setPpm(uint16_t){};
void LedStrip::go(state){};
#else

#include <Adafruit_NeoPixel.h>
#include "utils.h"

LedStrip::LedStrip() : _strip(Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800)),
                       _every500Millis(CheckTimeSpanPassed(500, false)) {}

void LedStrip::begin()
{
    _strip.begin();
    _strip.show();
    _strip.setBrightness(50);
}

void LedStrip::setPpm(uint16_t ppm)
{
    _ppm = ppm;
}

uint32_t LedStrip::translate(uint16_t ppm)
{
    if (ppm < 700u)
        return Adafruit_NeoPixel::Color(0, 0xFF, 0);
    else if (ppm < 1000u)
        return Adafruit_NeoPixel::Color(0xFF, 0xFF, 0);
    else if (ppm < 2000u)
        return Adafruit_NeoPixel::Color(0xFF, 0, 0);

    // all other values are >= 2000
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
        auto currentColor = translate(_ppm);
        if (currentColor != _color)
        {
            needsUpdate = true;
            _color = currentColor;
        }
        break;
    }
    case LedStrip::heating:
        needsUpdate = true;
        if (_blueish == 150 || _blueish == 0)
            direction = !direction;
        _color = Adafruit_NeoPixel::Color(0, 0, _blueish);
        direction ? _blueish-- : _blueish++;
        break;
    }

    if (needsUpdate)
    {
        for (size_t i = 0; i < _strip.numPixels(); i++)
        {
            _strip.setPixelColor(i, _color);
        }
        _strip.show();
    }
}

#endif