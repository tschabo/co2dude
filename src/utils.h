#pragma once
#include <Arduino.h>
#include "configuration.h"

class CheckTimeSpanPassed
{
public:
    CheckTimeSpanPassed(unsigned long timediff, bool fireOnFirstCall = false);
    bool operator()();

private:
    const unsigned long _timeDiff{};
    unsigned long _current{};
};