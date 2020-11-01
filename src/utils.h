#pragma once

class CheckTimeSpanPassed
{
public:
    CheckTimeSpanPassed(unsigned long timediff, bool fireOnFirstCall = false);
    bool operator()();

private:
    const unsigned long _timeDiff{};
    unsigned long _current{};
};