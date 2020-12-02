#pragma once
#include "robustDcf.h"

class DcfUtcClock
{
public:
    DcfUtcClock(uint32_t dcfPin, bool activeHigh, uint32_t disableDcfPin);
    void init();
    bool update();
    bool isLastSyncSuccessful();

private:
    const Chronos::Span::Days DEADRECKONING = 1;
    bool dcfReady();
    Chronos::DateTime lastSuccessfulSync;
    RobustDcf _rd;
    uint32_t _disableDcfPin;
};