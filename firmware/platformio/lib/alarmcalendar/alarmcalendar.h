/**
 * AlarmCalendar
 * 
 * It's a class that defines objest that can hold one alarm.  This alarm is determined by : 
 *  - a time of day when the alarm will go off : e.g. 0715h.
 *  - a list of days of the week on which the alarm will be enabled
 *  - the duration of the alarm
 * 
 * This calendar can work with local time or with UTC-time.  In the use case of an alarm clock, local time will be used because it's more practical.
 * You may want to get up at 7AM local time no matter whether it is daylight savings time or not.  
 * The calendar is programmed with a fixed hour & minute.  Using local time, this will always be correct.  Using UTC, the alarm would have to be adjusted two times a year for daylight savings time. 
 */

#pragma once
#include "Time.h"
#include "Chronos.h"

static const byte MAX_NR_OF_EVENTS = 7; // each day of the week can have one event
DefineCalendarType(Calendar, MAX_NR_OF_EVENTS);

typedef struct
{
  Chronos::Hours hour;
  Chronos::Minutes mins;
  bool weekdays[7];
} ALARM_CONFIG;

class AlarmCalendar
{
public:
  AlarmCalendar(byte durationMinutes);
  void getConfig(ALARM_CONFIG *config);
  bool setConfig(ALARM_CONFIG *config);
  void setDailyAlarm(Chronos::Hours hours, Chronos::Minutes minutes);
  void disableWeekday(Chronos::Weekday::Day aDay);
  void enableWeekday(Chronos::Weekday::Day aDay);
  bool setTime(Chronos::Hours hours, Chronos::Minutes minutes);
  bool getStartOfNextEvent(const Chronos::DateTime *timenow, Chronos::DateTime *returnDT);
  bool isAlarmOnGoing(const Chronos::DateTime *timenow);
  static byte dayToIndex(Chronos::Weekday::Day day);
  static Chronos::Weekday::Day indexToDay(byte index);
  void listEvents(Chronos::DateTime nowTime);
private:
  bool updateCalendar();
  Calendar _MyCalendar;
  ALARM_CONFIG _config;
  byte _duration;
};