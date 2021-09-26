#pragma once
#include "robustDcf.h"

class DcfUtcClock
{
public:
    DcfUtcClock(uint32_t dcfPin, bool activeHigh, uint32_t disableDcfPin);
    void init();
    bool update();
    bool isStillSynced();

private:
    const Chronos::Span::Days DEADRECKONING = 1;
    bool dcfReady();
    RobustDcf _rd;
    uint32_t _disableDcfPin;
    enum
    {
        SYNC_ONGOING,
        WAITING_FOR_SYNC
    } _syncState = WAITING_FOR_SYNC;
};