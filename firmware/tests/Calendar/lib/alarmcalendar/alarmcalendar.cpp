#include "alarmcalendar.h"

AlarmCalendar::AlarmCalendar() : _defaultSpan(2)
{
}

void AlarmCalendar::getConfig(ALARM_CONFIG *pconfig)
{
    memcpy(pconfig, &_config, sizeof(_config));
}

void AlarmCalendar::setConfig(ALARM_CONFIG *pconfig)
{
    memcpy(&_config, pconfig, sizeof(_config));
}

void AlarmCalendar::setDailyAlarm(Chronos::Hours hours, Chronos::Minutes minutes)
{
    //Enable bit 1 (Sunday) up to 7 (Saturday);
    _config.weekdays |= 0xFE;
    setTime(hours, minutes);
}

void AlarmCalendar::setAlarmCallBack(alarmCallBack function)
{
    _alarmCall = function;
}

bool AlarmCalendar::getStartOfNextEvent(Chronos::DateTime* returnDT)
{
    return _MyCalendar.nextDateTimeOfInterest(Chronos::DateTime::now(), *returnDT);
}

bool AlarmCalendar::isAlarmOnGoing()
{
    Chronos::Event::Occurrence occurrenceList[MAX_NR_OF_EVENTS];
    return _MyCalendar.listOngoing(MAX_NR_OF_EVENTS, occurrenceList, Chronos::DateTime::now()) > 0;
}

void AlarmCalendar::disableWeekday(Chronos::Weekday::Day aDay)
{
    //Chronos::Weekday::Day uses bits 1 to 7
    bitClear(_config.weekdays, aDay);
    updateCalendar();
}

void AlarmCalendar::enableWeekday(Chronos::Weekday::Day aDay)
{
    //Chronos::Weekday::Day uses bits 1 to 7
    bitSet(_config.weekdays, aDay);
    updateCalendar();
}

bool AlarmCalendar::setTime(Chronos::Hours hours, Chronos::Minutes minutes)
{
    _config.hour = hours;
    _config.mins = minutes;
    return updateCalendar();
}

bool AlarmCalendar::loop()
{
    if (!isAlarmOnGoing())
    {
        return false;
    }
    if (_alarmCall)
    {
        _alarmCall();
    }
    return true;
}

bool AlarmCalendar::updateCalendar()
{
    _MyCalendar.clear();
    for (byte day = Chronos::Weekday::Sunday; day <= Chronos::Weekday::Saturday; day++)
    {
        if (bitRead(_config.weekdays, day))
        {
            if (!_MyCalendar.add(Chronos::Event(day, Chronos::Mark::Weekly(day, _config.hour, _config.mins, 00), _defaultSpan)))
            {
                return false;
            }
        }
    }
    return true;
}
