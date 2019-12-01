#pragma once
#include "dcfUtcClock.h"
#include "AlarmManager.h"

typedef enum
{
    ALARM1,
    ALARM2,
    MAX_ALARMS
} ALARMNRS;

class Task_Time
{
public:
    Task_Time(DcfUtcClock* dcf);
    bool loop();
    time_t getLocalEpoch();
    byte getLocalHour();
    byte getLocalMinute();
    bool lastSyncOk();
private:
    DcfUtcClock* dcfclock;
    time_t localEpochSecs;
};

class Task_Alarms
{
public:
    Task_Alarms(AlarmManager **alarms);
    bool loop(time_t localEpochSecs);
    bool getSoonestAlarm(time_t localEpochSecs, AlarmConfig* soonestAlarm);
    void turnAlarmOff(time_t localEpochSecs);
private:
    AlarmManager **_alarms;
};