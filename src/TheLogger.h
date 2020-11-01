#pragma once

#include <SPI.h>
#include <SdFat.h>
#include <RTClib.h>

class TheLogger
{
public:
    TheLogger(const TheLogger &) = delete;
    TheLogger &operator=(const TheLogger &) = delete;
    TheLogger() = default;
    bool begin(const SPISettings& spiSettings);

    void log(const String &p);

private:
    // SD chip select pin.
    const uint8_t chipSelect = SS;
    SdFat _sd;

    // Log file.
    SdFile _file;

    DateTime _currentDate;
    char _filename[13]{};
};
