#include "robustDcf.h"

static volatile bool secondTicked = false;
static bool syncMark, longPulse;

RobustDcf::RobustDcf(const byte inputPin, const byte monitorPin) : _pd(inputPin, monitorPin),
                                                                   _minutes(21, 7, true, 0, 59, 4),
                                                                   _hours(29, 6, true, 0, 23, 3),
                                                                   _days(36, 6, false, 1, 31, 3),
                                                                   _months(45, 5, false, 1, 12, 2),
                                                                   _years(50, 8, false, 0, 99, 4)
{
}

//secondsTick is called by an ISR.  It should be kept as short as possible
static void secondsTick(const bool isSyncMark, const bool isLongPulse)
{
    syncMark = isSyncMark;
    longPulse = isLongPulse;
    secondTicked = true;
}

void RobustDcf::init()
{
    _pd.init(secondsTick);
}

bool RobustDcf::update(Chronos::EpochTime &unixEpoch)
{
    if (!secondTicked)
    {
        return false;
    }
    secondTicked = false;
    _sd.updateSeconds(syncMark, longPulse);
    uint8_t second;
    SecondsDecoder::BITDATA data;
    if ((!_sd.getSecond(second)) || (second != 59) || (!_sd.getTimeData(&data)))
    {
        return false;
    }
    return updateClock(&data , &unixEpoch);
}

bool RobustDcf::updateClock(SecondsDecoder::BITDATA *pdata, Chronos::EpochTime* pEpoch)
{
    bool bSuccess = true;
    bSuccess &= _minutes.update(pdata);
    bSuccess &= _hours.update(pdata);
    bSuccess &= _days.update(pdata);
    bSuccess &= _months.update(pdata);
    bSuccess &= _years.update(pdata);
    bSuccess &= _tzd.update(pdata);
    if (!bSuccess)
    {
        return false;
    }
    if (!getUnixEpochTime(pEpoch))
    {
        return false;
    }
    TimeChangeRule myDST = {"CEST", Last, Sun, Mar, 2, +120}; //Last Sunday of March, at 2AM, go to UTC+120min
    TimeChangeRule mySTD = {"CET", Last, Sun, Oct, 3, +60};   //Last Sunday of October, at 3AM, go to UTC+60min
    Timezone myTZ(myDST, mySTD);
    Chronos::DateTime localTime = myTZ.toLocal(*pEpoch);

    //set prediction for next minute
    Chronos::DateTime predictionTime = localTime + Chronos::Span::Minutes(1);
    _minutes.setPrediction(predictionTime.minute());
    _hours.setPrediction(predictionTime.hour());
    _days.setPrediction(predictionTime.day());
    _months.setPrediction(predictionTime.month());
    _years.setPrediction(tmYearToY2k(CalendarYrToTm(predictionTime.year())));
    _tzd.setPrediction(myTZ.locIsDST(predictionTime.asEpoch()));
    return true;
}

//Currently only minute resolution.
bool RobustDcf::getUnixEpochTime(Chronos::EpochTime* pUnixEpoch)
{
    uint8_t minute, hour, day, month, year;
    int16_t secondsOffset;
    if (_minutes.getTime(minute) && _hours.getTime(hour) && _days.getTime(day) && _months.getTime(month) && _years.getTime(year))
    {
        Chronos::DateTime localtime(tmYearToCalendar(y2kYearToTm(year)), month, day, hour, minute);
        _tzd.getSecondsOffset(secondsOffset);
        *pUnixEpoch = localtime.asEpoch() - secondsOffset;
        return true;
    }
    return false;
}