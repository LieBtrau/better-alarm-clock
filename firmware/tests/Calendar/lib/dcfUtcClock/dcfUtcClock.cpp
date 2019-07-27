#include "dcfUtcClock.h"
#include "time.h"
#include "stm32rtcwrapper.h"

static void dcfUpdatesRtc(bool start);
static volatile bool syncOngoing=false;
time_t getRtcTime();

static Stm32RtcWrapper stmRtc;


DcfUtcClock::DcfUtcClock(uint32_t dcfPin, bool activeHigh): syncCalendar(5), rd(dcfPin, activeHigh)
{
    syncCalendar.setDailyAlarm(2, 0);
    syncCalendar.setAlarmCallBack(dcfUpdatesRtc);
}

void DcfUtcClock::init()
{
    rd.init();
    stmRtc.begin();
    setSyncProvider(getRtcTime);                        // set time source for time.h
}


bool DcfUtcClock::update()
{
    bool mcuTimeValid = (timeStatus() == timeSet);
    if (!mcuTimeValid)
    {
        syncOngoing=true;
    }
    else
    {
        syncCalendar.loop();
    }
    if(syncOngoing)
    {
        Chronos::EpochTime epoch;
        syncOngoing = !dcfReady(epoch);
    }
    return mcuTimeValid;
}

// returns true when a valid DCF timestamp has been received
bool DcfUtcClock::dcfReady(Chronos::EpochTime& epoch)
{
    if(!rd.update(epoch))
    {
        return false;
    }
    if(stmRtc.setEpoch(epoch))                      //update RTC with DCF-data
    {
        setTime(epoch);                             //Update MCU-time by RTC: Force set time, otherwise it will only be set after 300s
    }
    return true;
}

time_t getRtcTime()
{
    return stmRtc.get();
}

void dcfUpdatesRtc(bool start)
{
    syncOngoing=start;
}


