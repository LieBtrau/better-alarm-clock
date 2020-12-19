#include "pins.h"
#include "dcfUtcClock.h"

static DcfUtcClock dcfclock(pin_DCF, true, pin_nEn_DCF);
// Central European Time (Frankfurt, Paris)
static TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Central European Summer Time
static TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};   // Central European Standard Time
static Timezone CE(CEST, CET);
static byte lastMinute = 0xFF;

void initClockSource()
{
    dcfclock.init();
}

/**
 *  \brief Run this function regularly to keep clock up to date, especially when no sync has been acquired yet.
 *  \return true when time is valid and if it has changed since the last call.
 */
bool getLocalTimeSeconds(time_t& locTime)
{
    if (!dcfclock.update())
    {
        return false;
    }
    time_t utc = now();
    TimeChangeRule *tcr; // pointer to the time change rule, use to get the TZ abbrev
    locTime = CE.toLocal(utc, &tcr);
    return true;
}

bool isNewMinuteStarted(time_t t, byte &hours, byte &minutes)
{
    if (lastMinute == minute(t))
    {
        return false;
    }
    lastMinute = minutes = minute(t);
    hours = hour(t);
    return true;
}

bool isStillSynced()
{
    return dcfclock.isLastSyncSuccessful();
}