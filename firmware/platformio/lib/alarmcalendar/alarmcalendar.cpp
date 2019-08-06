#include "alarmcalendar.h"

AlarmCalendar::AlarmCalendar(byte id, byte durationMinutes): _id(id), _duration(durationMinutes)
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
    for (byte i = 0; i < 7; i++)
    {
        _config.weekdays[i] = true;
    }
    setTime(hours, minutes);
}

//\brief alarmCallBack will be called with parameter true at the start of the event and with parameter false at the end of the event.
void AlarmCalendar::setAlarmCallBack(alarmCallBack function)
{
    _alarmCall = function;
}

bool AlarmCalendar::getStartOfNextEvent(const Chronos::DateTime *timenow, Chronos::DateTime *returnDT)
{
    return _MyCalendar.nextDateTimeOfInterest(*timenow, *returnDT);
}

bool AlarmCalendar::isAlarmOnGoing(const Chronos::DateTime *timenow)
{
    Chronos::Event::Occurrence occurrenceList[MAX_NR_OF_EVENTS];
    return _MyCalendar.listOngoing(MAX_NR_OF_EVENTS, occurrenceList, *timenow) > 0;
}

void AlarmCalendar::disableWeekday(Chronos::Weekday::Day aDay)
{
    //Chronos::Weekday::Day uses bits 1 to 7
    _config.weekdays[dayToIndex(aDay)] = false;
    updateCalendar();
}

void AlarmCalendar::enableWeekday(Chronos::Weekday::Day aDay)
{
    //Chronos::Weekday::Day uses bits 1 to 7
    _config.weekdays[dayToIndex(aDay)] = true;
    updateCalendar();
}

bool AlarmCalendar::setTime(Chronos::Hours hours, Chronos::Minutes minutes)
{
    _config.hour = hours;
    _config.mins = minutes;
    return updateCalendar();
}

bool AlarmCalendar::loop(const Chronos::DateTime *timenow)
{
    bool alarmIsOn = isAlarmOnGoing(timenow);
    if (_alarmCall)
    {
        if (alarmIsOn && !_alarmWasOn)
        {
            _alarmCall(_id, true);
        }
        if (!alarmIsOn && _alarmWasOn)
        {
            _alarmCall(_id, false);
        }
        _alarmWasOn = alarmIsOn;
    }
    return isAlarmOnGoing(timenow);
}

bool AlarmCalendar::updateCalendar()
{
    _MyCalendar.clear();
    for (byte index = 0; index < 7; index++)
    {
        if (_config.weekdays[index])
        {
            if (!_MyCalendar.add(Chronos::Event(indexToDay(index), Chronos::Mark::Weekly(indexToDay(index), _config.hour, _config.mins, 00), Chronos::Span::Minutes(_duration))))
            {
                return false;
            }
        }
    }
    return true;
}

void AlarmCalendar::listEvents(Chronos::DateTime nowTime)
{
    const byte OCCURRENCES_LIST_SIZE=30;
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

Chronos::Weekday::Day AlarmCalendar::indexToDay(byte index)
{
    switch (index)
    {
    default:
    case 0:
        return Chronos::Weekday::Monday;
    case 1:
        return Chronos::Weekday::Tuesday;
    case 2:
        return Chronos::Weekday::Wednesday;
    case 3:
        return Chronos::Weekday::Thursday;
    case 4:
        return Chronos::Weekday::Friday;
    case 5:
        return Chronos::Weekday::Saturday;
    case 6:
        return Chronos::Weekday::Sunday;
    }
}

byte AlarmCalendar::dayToIndex(Chronos::Weekday::Day day)
{
    switch (day)
    {
    default:
    case Chronos::Weekday::Monday:
        return 0;
    case Chronos::Weekday::Tuesday:
        return 1;
    case Chronos::Weekday::Wednesday:
        return 2;
    case Chronos::Weekday::Thursday:
        return 3;
    case Chronos::Weekday::Friday:
        return 4;
    case Chronos::Weekday::Saturday:
        return 5;
    case Chronos::Weekday::Sunday:
        return 6;
    }
}