#include "TheLogger.h"

bool TheLogger::begin(const SPISettings& spiSettings)
{
    // Initialize at the highest speed supported by the board that is
    // not over 50 MHz. Try a lower speed if SPI errors occur.
    if (!_sd.begin(chipSelect, spiSettings))
    {
        return false;
    }
    return true;
}

void TheLogger::log(const String &p)
{
    auto now = RTC_DS1307::now();
    auto nowDate = DateTime(now.year(), now.month(), now.day(), 0, 0, 0);
    if (nowDate > _currentDate)
    {
        if (_file.isOpen())
        {
            _file.sync();
            _file.close();
        }
        snprintf(_filename, sizeof(_filename), "%d%02d%02d.log", now.year(), now.month(), now.day());
        _file.open(_filename, O_WRONLY | O_SYNC | O_APPEND | O_CREAT);
        _currentDate = nowDate;
    }
    char dateTime[20]{};
    snprintf(dateTime, sizeof(dateTime), "%d/%02d/%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
    _file.print(dateTime);
    _file.print(";");
    _file.print(p);
    _file.println();
}
