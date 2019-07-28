/**
 * AlarmCalendar
 * 
 * It's a class that defines objest that can hold one alarm.  This alarm is determined by : 
 *  - a time of day when the alarm will go off : e.g. 0715h.
 *  - a list of days of the week on which the alarm will be enabled
 *  - the duration of the alarm
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
  byte weekdays;
  byte duration;
} ALARM_CONFIG;

class AlarmCalendar
{
public:
  typedef void (*alarmCallBack)(bool bIsAlarmStartNotEnd);
  AlarmCalendar(byte durationMinutes);
  void getConfig(ALARM_CONFIG *config);
  void setConfig(ALARM_CONFIG *config);
  void setDailyAlarm(Chronos::Hours hours, Chronos::Minutes minutes);
  void disableWeekday(Chronos::Weekday::Day aDay);
  void enableWeekday(Chronos::Weekday::Day aDay);
  bool setTime(Chronos::Hours hours, Chronos::Minutes minutes);
  bool getStartOfNextEvent(const Chronos::DateTime* timenow, Chronos::DateTime *returnDT);
  bool isAlarmOnGoing(const Chronos::DateTime *timenow);
  void setAlarmCallBack(alarmCallBack function);
  bool loop(const Chronos::DateTime* timenow);

private:
  bool updateCalendar();
  Calendar _MyCalendar;
  ALARM_CONFIG _config;
  alarmCallBack _alarmCall = nullptr;
  bool _alarmWasOn = false;
};