#include "dcfUtcClock.h"
#include "time.h"
#include "stm32rtcwrapper.h"

static volatile bool syncOngoing = false;
time_t getRtcTime();

static Stm32RtcWrapper stmRtc;

DcfUtcClock::DcfUtcClock(uint32_t dcfPin, bool activeHigh) : syncCalendar(5), _rd(dcfPin, activeHigh)
{
    syncCalendar.setDailyAlarm(0, 0); //sync at UTC midnight
}

void DcfUtcClock::init()
{
    _rd.init();
    stmRtc.begin();
    setSyncProvider(getRtcTime); // set time source for time.h
}

/**  \brief returns true when MCU-time is valid
 */ 
bool DcfUtcClock::update()
{
    bool mcuTimeValid = (timeStatus() == timeSet);
    if (!mcuTimeValid)
    {
        syncOngoing = true;
    }
    else
    {
        Chronos::DateTime timenow = Chronos::DateTime::now();
        syncOngoing = syncCalendar.isAlarmOnGoing(&timenow);
    }
    if (syncOngoing)
    {
        Chronos::EpochTime epoch;
        syncOngoing = !dcfReady(epoch);
        _lastSyncSuccessful = !syncOngoing;
    }
    return mcuTimeValid;
}

// returns true when a valid DCF timestamp has been received
bool DcfUtcClock::dcfReady(Chronos::EpochTime &epoch)
{
    if (!_rd.update(epoch))
    {
        return false;
    }
    if (stmRtc.setEpoch(epoch)) //update RTC with DCF-data
    {
        setTime(epoch); //Update MCU-time by RTC: Force set time, otherwise it will only be set after 300s
    }
    return true;
}

bool DcfUtcClock::isLastSyncSuccessful()
{
    return _lastSyncSuccessful;
}

time_t getRtcTime()
{
    return stmRtc.get();
}
