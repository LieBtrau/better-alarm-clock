/**
 * AlarmCalendar
 * 
 * It's a class that defines an object that can hold one alarm.  This alarm is determined by : 
 *  - a time of day when the alarm will go off : e.g. 07h15.
 *  - a list of days of the week on which the alarm will be enabled
 *  - the duration of the alarm in minutes
 * 
 * This calendar is time zone agnostic.  In the use case of an alarm clock, local time will be used because it's more practical.
 *  You may want to get up at 7AM local time no matter whether it is daylight savings time or not.  
 *  The calendar is programmed with a fixed hour & minute.  Using local time, this will always be correct.  
 *  Using UTC, the alarm would have to be adjusted two times a year for daylight savings time. 
 */

#pragma once
#include "Time.h"
#include "Chronos.h"

static const byte MAX_NR_OF_EVENTS = 7; // each day of the week can have one event
DefineCalendarType(Calendar, MAX_NR_OF_EVENTS);

typedef struct
{
  Chronos::Hours hour = 0;
  Chronos::Minutes mins = 0;
  byte weekdays = 0;
} ALARM_CONFIG;

class AlarmCalendar
{
public:
  enum WEEKDAYS
  {
    WD_NODAY = 0x00,
    WD_MONDAY = 0x01,
    WD_TUESDAY = 0x02,
    WD_WEDNESDAY = 0x04,
    WD_THURSDAY = 0x08,
    WD_FRIDAY = 0x10,
    WD_SATURDAY = 0x20,
    WD_SUNDAY = 0x40
  };
  AlarmCalendar(byte alarmDuration_minutes);
  void getConfig(ALARM_CONFIG *config);
  bool setConfig(ALARM_CONFIG *config);
  void setDailyAlarm(Chronos::Hours hours, Chronos::Minutes minutes);
  void setWeekdays(WEEKDAYS wd);
  bool setAlarmTime(Chronos::Hours hours, Chronos::Minutes minutes);
  bool getSecondsToStartOfNextEvent(time_t tNow, time_t &totalSecondsToNextEvent);
  bool isAlarmIn24Hours(time_t tNow);
  bool isUnacknowledgedAlarmOnGoing(time_t t);
  void acknowledgeAlarm();
  void listEvents(Chronos::DateTime nowTime);

private:
  Chronos::Weekday::Day indexToDay(int index);
  bool updateCalendar();
  Calendar _MyCalendar;
  ALARM_CONFIG _config;
  byte _duration;
  bool _alarmAcked = false;
};

