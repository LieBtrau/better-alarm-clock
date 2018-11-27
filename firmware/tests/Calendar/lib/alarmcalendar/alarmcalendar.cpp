#include "alarmcalendar.h"

AlarmCalendar::AlarmCalendar():
    _defaultSpan(90)
{}

bool AlarmCalendar::addOnceOnlyEvent(ALARMNR nr, Chronos::Weekday::Day aDay, Chronos::Hours hours, Chronos::Minutes minutes)
{
    _MyCalendar.clear();

    Chronos::Mark::Weekly everyWeekMark(aDay, hours, minutes);
    Chronos::DateTime nowTime(Chronos::DateTime::now());
    Chronos::DateTime nextOccurence = nowTime.next(everyWeekMark);
    _MyCalendar.add(Chronos::Event(nr*MAX_AlarmType + OnceOnly, nextOccurence, _defaultSpan));
}

bool AlarmCalendar::addDailyEvent(ALARMNR nr, Chronos::Hours hours, Chronos::Minutes minutes)
{
    _MyCalendar.clear();

    _MyCalendar.add(Chronos::Event(nr*MAX_AlarmType + Daily, Chronos::Mark::Daily(hours, minutes), _defaultSpan));
}

bool AlarmCalendar::addWeeklyEvent(ALARMNR nr, Chronos::Weekday::Day aDay, Chronos::Hours hours, Chronos::Minutes minutes)
{
    _MyCalendar.remove(nr*MAX_AlarmType + OnceOnly);
    _MyCalendar.remove(nr*MAX_AlarmType + Daily);
    _MyCalendar.remove(nr*MAX_AlarmType + getAlarmType(aDay));

    _MyCalendar.add(Chronos::Event(nr*MAX_AlarmType + getAlarmType(aDay), Chronos::Mark::Weekly(aDay, hours, minutes), _defaultSpan));
}

bool AlarmCalendar::getStartOfNextEvent(Chronos::DateTime & returnDT)
{
    return _MyCalendar.nextDateTimeOfInterest(Chronos::DateTime::now(), returnDT);
}

AlarmCalendar::AlarmType AlarmCalendar::getAlarmType(Chronos::Weekday::Day aDay)
{
    switch (aDay)
    {
    case Chronos::Weekday::Sunday:
        return WeeklySunday;
    case Chronos::Weekday::Monday:
        return WeeklyMonday;
    case Chronos::Weekday::Tuesday:
        return WeeklyTuesday;
    case Chronos::Weekday::Wednesday:
        return WeeklyWednesday;
    case Chronos::Weekday::Thursday:
        return WeeklyThursday;
    case Chronos::Weekday::Friday:
        return WeeklyFriday;
    case Chronos::Weekday::Saturday:
        return WeeklySaturday;
    default:
        return Invalid;
    }
}
