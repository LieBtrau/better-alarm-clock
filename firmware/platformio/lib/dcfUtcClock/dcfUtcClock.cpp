#include "dcfUtcClock.h"
#include "time.h"
#include "stm32rtcwrapper.h"

time_t getRtcTime();

static Stm32RtcWrapper stmRtc;

DcfUtcClock::DcfUtcClock(uint32_t dcfPin, bool activeHigh, uint32_t disableDcfPin) : _rd(dcfPin, activeHigh),
                                                                                     _disableDcfPin(disableDcfPin)
{
}

void DcfUtcClock::init()
{
    pinMode(_disableDcfPin, OUTPUT);
    digitalWrite(_disableDcfPin, LOW);
    _rd.init();
    stmRtc.begin();
    setSyncProvider(getRtcTime); // set time source for time.h
}

/**  \brief  Needs to be called regularly to run the clock update process
 *   \return true when MCU-time is valid
 */
bool DcfUtcClock::update()
{
    Chronos::EpochTime rtcepoch;
    //both millis() timer and RTC need to be set
    bool mcuTimeValid = (timeStatus() == timeSet) && stmRtc.get(rtcepoch);
    Chronos::DateTime timenow = Chronos::DateTime::now();
    bool syncNeeded = mcuTimeValid ? timenow.hour() == 2 && timenow - lastSuccessfulSync > Chronos::Span::Hours(20) : true;
    if (syncNeeded)
    {
        digitalWrite(_disableDcfPin, LOW);
        if (_rd.update(rtcepoch))
        {
            stmRtc.setEpoch(rtcepoch); //update RTC with DCF-data
            setTime(rtcepoch);         //Update MCU-time by RTC: Force set time, otherwise it will only be set after 300s
            lastSuccessfulSync = Chronos::DateTime(rtcepoch);
            digitalWrite(_disableDcfPin, HIGH);
        }
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
