#pragma once
#include <Time.h>
#include <Chronos.h>

static const byte MAX_NR_OF_EVENTS = 7; // +1 = extra event for syncing clock
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
  bool getStartOfNextEvent(Chronos::DateTime *returnDT);
  bool isAlarmOnGoing();
  void setAlarmCallBack(alarmCallBack function);
  bool loop();

private:
  bool updateCalendar();
  Calendar _MyCalendar;
  ALARM_CONFIG _config;
  alarmCallBack _alarmCall = nullptr;
  bool _alarmWasOn = false;
};