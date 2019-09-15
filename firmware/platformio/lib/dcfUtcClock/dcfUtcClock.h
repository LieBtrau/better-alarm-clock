#pragma once
#include "alarmcalendar.h"
#include "robustDcf.h"

class DcfUtcClock
{
public:
    DcfUtcClock(uint32_t dcfPin, bool activeHigh);
    void init();
    bool update();
    bool isLastSyncSuccessful();
private:
    bool dcfReady(Chronos::EpochTime& epoch);
    bool _lastSyncSuccessful = false;
    AlarmCalendar syncCalendar;
    RobustDcf _rd;
};