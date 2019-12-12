#include "dcfUtcClock.h"
#include "time.h"
#include "stm32rtcwrapper.h"

time_t getRtcTime();

static Stm32RtcWrapper stmRtc;

DcfUtcClock::DcfUtcClock(uint32_t dcfPin, bool activeHigh) : _rd(dcfPin, activeHigh)
{
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
    Chronos::EpochTime rtcepoch;
    //both millis() timer and RTC need to be set
    bool mcuTimeValid = (timeStatus() == timeSet) && stmRtc.get(rtcepoch);
    bool syncNeeded = !mcuTimeValid;
    if (mcuTimeValid)
    {
        Chronos::DateTime timenow = Chronos::DateTime::now();
        syncNeeded = timenow.hour() == 2 && timenow - lastSuccessfulSync > Chronos::Span::Hours(20);
    }
    if (syncNeeded && _rd.update(rtcepoch))
    {
        stmRtc.setEpoch(rtcepoch); //update RTC with DCF-data
        setTime(rtcepoch);         //Update MCU-time by RTC: Force set time, otherwise it will only be set after 300s
        lastSuccessfulSync = Chronos::DateTime(rtcepoch);
    }
    return mcuTimeValid;
}

bool DcfUtcClock::isLastSyncSuccessful()
{
    Chronos::DateTime timenow = Chronos::DateTime::now();
    return timenow - lastSuccessfulSync < DEADRECKONING;
}

time_t getRtcTime()
{
    Chronos::EpochTime epoch;
    return stmRtc.get(epoch) ? epoch : 0;
}
