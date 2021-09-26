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
    Chronos::DateTime utctimenow = Chronos::DateTime::now();
    bool mcuTimeValid = (timeStatus() == timeSet) && stmRtc.get(rtcepoch); //stmRtc will return false after DEADRECKONING timeout
    switch (_syncState)
    {
    case WAITING_FOR_SYNC:
        //both millis() timer and RTC need to be set
        if (!mcuTimeValid || (utctimenow.hour() == 0 && utctimenow.minute() == 0))
        {
            Serial.println("Start syncing");
            digitalWrite(_disableDcfPin, LOW);
            _syncState = SYNC_ONGOING;
        }
        break;
    case SYNC_ONGOING:
        if (_rd.update(rtcepoch))
        {
            Serial.println("Syncing done.");
            stmRtc.setEpoch(rtcepoch); //update RTC with DCF-data
            setTime(rtcepoch);         //Update MCU-time by RTC: Force set time, otherwise it will only be set after 300s
            digitalWrite(_disableDcfPin, HIGH);
            _syncState = WAITING_FOR_SYNC;
        }
        break;
    }
    return mcuTimeValid;
}

time_t getRtcTime()
{
    Chronos::EpochTime epoch;
    return stmRtc.get(epoch) ? epoch : 0;
}

bool DcfUtcClock::isStillSynced()
{
    return getRtcTime() != 0;
}