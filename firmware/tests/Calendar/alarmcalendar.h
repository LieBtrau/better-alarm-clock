#ifndef ALARMCALENDAR_H
#define ALARMCALENDAR_H

#include <Time.h>
#include <Chronos.h>

#define CALENDAR_MAX_NUM_EVENTS   14
DefineCalendarType(Calendar, CALENDAR_MAX_NUM_EVENTS);

class AlarmCalendar
{
public:
    AlarmCalendar();
    typedef enum{ALARM1, ALARM2, MAX_ALARM}ALARMNR;
    bool addOnceOnlyEvent(ALARMNR nr, Chronos::Weekday::Day aDay, Chronos::Hours hours, Chronos::Minutes minutes);
    bool addDailyEvent(ALARMNR nr, Chronos::Hours hours, Chronos::Minutes minutes);
    bool addWeeklyEvent(ALARMNR nr, Chronos::Weekday::Day aDay, Chronos::Hours hours, Chronos::Minutes minutes);
    bool removeOnceOnlyEvent(ALARMNR nr);
    bool removeDailyEvent(ALARMNR nr);
    bool removeWeeklyEvent(ALARMNR nr, Chronos::Weekday::Day aDay);
    bool getStartOfNextEvent(Chronos::DateTime &returnDT);
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
    }AlarmType;
    AlarmType getAlarmType(Chronos::Weekday::Day aDay);
    Calendar _MyCalendar;
    Chronos::Span::Minutes _defaultSpan;
};

#endif // ALARMCALENDAR_H
