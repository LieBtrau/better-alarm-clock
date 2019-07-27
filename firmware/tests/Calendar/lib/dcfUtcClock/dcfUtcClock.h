#pragma once
#include "alarmcalendar.h"
#include "robustDcf.h"

class DcfUtcClock
{
public:
    DcfUtcClock(uint32_t dcfPin, bool activeHigh);
    void init();
    bool update();
private:
    bool dcfReady(Chronos::EpochTime& epoch);
    AlarmCalendar syncCalendar;
    RobustDcf rd;
};