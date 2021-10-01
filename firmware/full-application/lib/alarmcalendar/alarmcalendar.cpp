#include "alarmcalendar.h"

AlarmCalendar::AlarmCalendar(byte alarmDuration_minutes) : _duration(alarmDuration_minutes)
{
}

void AlarmCalendar::getConfig(ALARM_CONFIG *pconfig)
{
    memcpy(pconfig, &_config, sizeof(_config));
}

bool AlarmCalendar::setConfig(ALARM_CONFIG *pconfig)
{
    memcpy(&_config, pconfig, sizeof(_config));
    return updateCalendar();
}

void AlarmCalendar::setDailyAlarm(Chronos::Hours hours, Chronos::Minutes minutes)
{
    //Enable bit 1 (Sunday) up to 7 (Saturday);
    _config.weekdays = (WEEKDAYS)(WD_MONDAY | WD_TUESDAY | WD_WEDNESDAY | WD_THURSDAY | WD_FRIDAY | WD_SATURDAY | WD_SUNDAY);
    setAlarmTime(hours, minutes);
    updateCalendar();
}

/**
 * \brief 
 */
bool AlarmCalendar::getSecondsToStartOfNextEvent(time_t tNow, time_t &totalSecondsToNextEvent)
{
    Chronos::DateTime tChronosNow(tNow);
    Chronos::Event::Occurrence occurrenceList[1];
    if (!_MyCalendar.listNext(1, occurrenceList, tChronosNow))
    {
        return false;
    }
    Chronos::Span::Absolute diff = occurrenceList[0].start - tChronosNow;
    totalSecondsToNextEvent = diff.totalSeconds();
    return true;
}

bool AlarmCalendar::isAlarmIn24Hours(time_t tNow)
{
    Chronos::DateTime tChronosNow(tNow);
    Chronos::Event::Occurrence occurrenceList[1];
    if (!_MyCalendar.listNext(1, occurrenceList, tChronosNow))
    {
        return false;
    }
    Chronos::Span::Delta diff = occurrenceList[0].start - tChronosNow;
    return diff.days() == 0;
}

bool AlarmCalendar::isUnacknowledgedAlarmOnGoing(time_t t)
{
    Chronos::DateTime timenow(t);
    Chronos::Event::Occurrence occurrenceList[MAX_NR_OF_EVENTS];
    bool alarmOngoing = _MyCalendar.listOngoing(MAX_NR_OF_EVENTS, occurrenceList, timenow) > 0;
    if (!alarmOngoing)
    {
        _alarmAcked = false; //reset alarm acknowledgement
        return false;
    }
    return !_alarmAcked;
}

void AlarmCalendar::acknowledgeAlarm()
{
    _alarmAcked = true;
}

void AlarmCalendar::setWeekdays(WEEKDAYS wd)
{
    _config.weekdays = wd;
    updateCalendar();
}

bool AlarmCalendar::setAlarmTime(Chronos::Hours hours, Chronos::Minutes minutes)
{
    _config.hour = hours;
    _config.mins = minutes;
    return updateCalendar();
}

bool AlarmCalendar::updateCalendar()
{
    _MyCalendar.clear();
    for (byte index = 0; index < 7; index++)
    {
        if (_config.weekdays & (1 << index))
        {
            if (!_MyCalendar.add(
                    Chronos::Event(indexToDay(index), Chronos::Mark::Weekly(indexToDay(index), _config.hour, _config.mins, 00),
                                   Chronos::Span::Minutes(_duration))))
            {
                return false;
            }
        }
    }
    return true;
}

void AlarmCalendar::listEvents(Chronos::DateTime nowTime)
{
    const byte OCCURRENCES_LIST_SIZE = 30;
    Chronos::Event::Occurrence occurrenceList[OCCURRENCES_LIST_SIZE];
    Serial.println("Coming up:");

    int numUpcoming = _MyCalendar.listNext(OCCURRENCES_LIST_SIZE, occurrenceList,
                                           nowTime);
    if (numUpcoming)
    {
        for (int i = 0; i < numUpcoming; i++)
        {
            Chronos::Span::Absolute startsIn = (occurrenceList[i].start - nowTime);
            if (startsIn < Chronos::Span::Days(3))
            {
                // coming soon, print a detailed listing...
                Serial.print("Event ");
                Serial.print((int)occurrenceList[i].id);
                Serial.print('\t');
                occurrenceList[i].start.printTo(Serial);
                Serial.print(" - ");
                occurrenceList[i].finish.printTo(Serial);

                Serial.print("Starts in ");
                startsIn.printTo(Serial);
            }
            else
            {
                // further into the future, just print summary
                Serial.print("Event ");
                Serial.print((int)occurrenceList[i].id);
                Serial.print('\t');
                occurrenceList[i].start.printTo(Serial);
                Serial.println();
            }
        }
    }
    else
    {
        // nothing returned by listNext()
        Serial.println("Calendar is wiiiide open... Anyone got plans?");
    }
}

Chronos::Weekday::Day AlarmCalendar::indexToDay(int index)
{
    WEEKDAYS wd = (WEEKDAYS)(1 << index);
    switch (wd)
    {
    default:
    case WD_MONDAY:
        return Chronos::Weekday::Monday;
    case WD_TUESDAY:
        return Chronos::Weekday::Tuesday;
    case WD_WEDNESDAY:
        return Chronos::Weekday::Wednesday;
    case WD_THURSDAY:
        return Chronos::Weekday::Thursday;
    case WD_FRIDAY:
        return Chronos::Weekday::Friday;
    case WD_SATURDAY:
        return Chronos::Weekday::Saturday;
    case WD_SUNDAY:
        return Chronos::Weekday::Sunday;
    }
}