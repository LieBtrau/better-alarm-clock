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
    Task_Time(DcfUtcClock *dcf);
    bool loop();
    time_t getLocalEpoch();
    byte getLocalHour();
    byte getLocalMinute();
    bool lastSyncOk();

private:
    DcfUtcClock *dcfclock;
    time_t localEpochSecs;
    // Central European Time (Frankfurt, Paris)
    TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Central European Summer Time
    TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};   // Central European Standard Time
    Timezone CE;
};

class Task_Alarms
{
public:
    Task_Alarms(AlarmManager *alarms);
    bool loop(time_t localEpochSecs);
    bool getSoonestAlarm(time_t localEpochSecs, byte &soonestAlarm);
    void turnAlarmOff(time_t localEpochSecs);

private:
    AlarmManager *_alarms;
};