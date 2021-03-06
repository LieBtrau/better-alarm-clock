#include "Tasks.h"

Task_Time::Task_Time(DcfUtcClock *dcf) : dcfclock(dcf), CE(CEST, CET) {}

/**
 * \brief       Returns true when MCU-time is synced to DCF
 */
bool Task_Time::loop()
{
    return dcfclock->update();
}

time_t Task_Time::getLocalEpoch()
{
    if (!loop())
    {
        return 0;
    }
    time_t utc = now();
    TimeChangeRule *tcr;
    localEpochSecs = CE.toLocal(utc, &tcr);
    return localEpochSecs;
}

byte Task_Time::getLocalHour()
{
    if (!loop())
    {
        return 0;
    }
    return hour(localEpochSecs);
}

byte Task_Time::getLocalMinute()
{
    if (!loop())
    {
        return 0;
    }
    return minute(localEpochSecs);
}

bool Task_Time::lastSyncOk()
{
    return dcfclock->isLastSyncSuccessful();
}

Task_Alarms::Task_Alarms(AlarmManager *alarms) : _alarms(alarms)
{
}

/**
 * \return  True when alarm is ongoing
 */
bool Task_Alarms::loop(time_t localEpochSecs)
{
    Chronos::DateTime localTime(localEpochSecs);
    bool alarmOngoing = false;
    for (byte i = 0; i < MAX_ALARMS; i++)
    {
        alarmOngoing |= _alarms[i].loop(&localTime);
    }
    return alarmOngoing;
}

bool Task_Alarms::getSoonestAlarm(time_t localEpochSecs, byte &soonestAlarmIndex)
{
    Chronos::Span::Days remainingUntilNextAlarm = Chronos::Span::Days(1);
    Chronos::DateTime localTime(localEpochSecs);
    Chronos::DateTime earliestAlarm = localTime + remainingUntilNextAlarm;
    Chronos::DateTime tempAlarm;
    soonestAlarmIndex = 0xFF;
    bool alarmFound = false;
    for (byte i = 0; i < MAX_ALARMS; i++)
    {
        if (_alarms[i].getAlarmFromNowWithinSpan(localTime, remainingUntilNextAlarm, tempAlarm) && tempAlarm < earliestAlarm)
        {
            earliestAlarm = tempAlarm;
            soonestAlarmIndex = i;
            alarmFound = true;
        }
    }
    return alarmFound;
}

void Task_Alarms::turnAlarmOff(time_t localEpochSecs)
{
    Chronos::DateTime localTime(localEpochSecs);
    for (byte i = 0; i < MAX_ALARMS; i++)
    {
        if (_alarms[i].loop(&localTime))
        {
            _alarms[i].turnAlarmOff();
        }
    }
}