#include "alarmcalendar.h"

AlarmCalendar::AlarmCalendar() : _defaultSpan(90)
{
}

void AlarmCalendar::setTimeSync(Chronos::Hours dailySyncHour, syncCallBack function)
{
    _MyCalendar.add(Chronos::Event(SYNC_EVENT, Chronos::Mark::Daily(dailySyncHour, 00, 00), Chronos::Span::Minutes(5)));
    _syncCall = function;
}

void AlarmCalendar::setAlarmCallBack(alarmCallBack function)
{
    _alarmCall = function;
}

bool AlarmCalendar::addOnceOnlyEvent(ALARMNR nr, Chronos::Weekday::Day aDay, Chronos::Hours hours, Chronos::Minutes minutes)
{
    _MyCalendar.clear();

    Chronos::Mark::Weekly everyWeekMark(aDay, hours, minutes);
    Chronos::DateTime nowTime(Chronos::DateTime::now());
    Chronos::DateTime nextOccurence = nowTime.next(everyWeekMark);
    return _MyCalendar.add(Chronos::Event(getEventSlot(nr, OnceOnly), nextOccurence, _defaultSpan));
}

bool AlarmCalendar::addDailyEvent(ALARMNR nr, Chronos::Hours hours, Chronos::Minutes minutes)
{
    _MyCalendar.clear();

    return _MyCalendar.add(Chronos::Event(getEventSlot(nr, Daily), Chronos::Mark::Daily(hours, minutes), _defaultSpan));
}

bool AlarmCalendar::addWeeklyEvent(ALARMNR nr, Chronos::Weekday::Day aDay, Chronos::Hours hours, Chronos::Minutes minutes)
{
    _MyCalendar.remove(getEventSlot(nr, OnceOnly));
    _MyCalendar.remove(getEventSlot(nr, Daily));
    _MyCalendar.remove(getEventSlot(nr, getAlarmType(aDay)));

    return _MyCalendar.add(Chronos::Event(getEventSlot(nr, getAlarmType(aDay)), Chronos::Mark::Weekly(aDay, hours, minutes), _defaultSpan));
}

bool AlarmCalendar::getStartOfNextEvent(Chronos::DateTime &returnDT)
{
    return _MyCalendar.nextDateTimeOfInterest(Chronos::DateTime::now(), returnDT);
}

void AlarmCalendar::checkForOngoingEvents()
{
    Chronos::Event::Occurrence occurrenceList[MAX_NR_OF_EVENTS];
    byte numOngoing = _MyCalendar.listOngoing(MAX_NR_OF_EVENTS, occurrenceList, Chronos::DateTime::now());
    for (byte i = 0; i < numOngoing; i++)
    {
        if (_alarmCall)
        {
            if (occurrenceList[i].id == SYNC_EVENT)
            {
                if (_syncCall != nullptr)
                {
                    Chronos::EpochTime epoch;
                    _syncCall(epoch);
                }
            }
            else
            {
                if (_alarmCall != nullptr)
                {
                    _alarmCall(occurrenceList[i].id > EVENTSLOTS_PER_ALARM ? ALARM2 : ALARM1);
                }
            }
        }
    }
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

//EventSlot numbers start at 1, not at 0
byte AlarmCalendar::getEventSlot(ALARMNR alarm, AlarmType type)
{
    byte offset = 0;
    switch (type)
    {
    case OnceOnly:
    case Daily:
    case WeeklyMonday:
        offset = 1;
        break;
    case WeeklyTuesday:
        offset = 2;
        break;
    case WeeklyWednesday:
        offset = 3;
        break;
    case WeeklyThursday:
        offset = 4;
        break;
    case WeeklyFriday:
        offset = 5;
        break;
    case WeeklySaturday:
        offset = 6;
        break;
    case WeeklySunday:
        offset = 7;
        break;
    default:
        offset = 0;
    }
    return alarm * EVENTSLOTS_PER_ALARM + offset;
}
