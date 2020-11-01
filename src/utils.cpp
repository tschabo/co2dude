#include "utils.h"
#include <Arduino.h>

CheckTimeSpanPassed::CheckTimeSpanPassed(unsigned long timediff, bool fireOnFirstCall) : _timeDiff(timediff), _current(millis() - (fireOnFirstCall ? timediff : 0)) {}

bool CheckTimeSpanPassed::operator()()
{
    auto now = millis();
    unsigned long diff = (_current > now) ? (std::numeric_limits<unsigned long>::max() - _current + now) : (now - _current);
    if (diff >= _timeDiff)
    {
      if (diff > 2 * _timeDiff)
        Serial.println("time diff overflow");
      _current = now - (diff - _timeDiff);
      return true;
    }
    return false;
  }