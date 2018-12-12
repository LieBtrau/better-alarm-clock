#pragma once
#include <Time.h>
#include <Chronos.h>

typedef enum
{
    ALARM1,
    ALARM2,
    MAX_ALARM
} ALARMNR;
const byte EVENTSLOTS_PER_ALARM = 7;
const byte MAX_NR_OF_EVENTS = MAX_ALARM * EVENTSLOTS_PER_ALARM + 1;
const byte SYNC_EVENT = MAX_NR_OF_EVENTS;
DefineCalendarType(Calendar, MAX_NR_OF_EVENTS);

class AlarmCalendar
{
  public:
    typedef void (*alarmCallBack)(ALARMNR nr);
    typedef bool (*syncCallBack)(Chronos::EpochTime &epoch);
    AlarmCalendar();
    bool addOnceOnlyEvent(ALARMNR nr, Chronos::Weekday::Day aDay, Chronos::Hours hours, Chronos::Minutes minutes);
    bool addDailyEvent(ALARMNR nr, Chronos::Hours hours, Chronos::Minutes minutes);
    bool addWeeklyEvent(ALARMNR nr, Chronos::Weekday::Day aDay, Chronos::Hours hours, Chronos::Minutes minutes);
    bool removeOnceOnlyEvent(ALARMNR nr);
    bool removeDailyEvent(ALARMNR nr);
    bool removeWeeklyEvent(ALARMNR nr, Chronos::Weekday::Day aDay);
    bool getStartOfNextEvent(Chronos::DateTime &returnDT);
    void setTimeSync(Chronos::Hours dailySyncHour, syncCallBack function);
    void setAlarmCallBack(alarmCallBack function);
    void checkForOngoingEvents();

  private:
    typedef enum
    {
        Invalid,
        OnceOnly,
        Daily,
        WeeklyMonday,
        WeeklyTuesday,
        WeeklyWednesday,
        WeeklyThursday,
        WeeklyFriday,
        WeeklySaturday,
        WeeklySunday,
        MAX_AlarmType
    } AlarmType;
    AlarmType getAlarmType(Chronos::Weekday::Day aDay);
    byte getEventSlot(ALARMNR alarm, AlarmType type);
    Calendar _MyCalendar;
    Chronos::Span::Minutes _defaultSpan;
    alarmCallBack _alarmCall = nullptr;
    syncCallBack _syncCall = nullptr;
};