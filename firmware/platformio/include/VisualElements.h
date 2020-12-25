/* This class contains functionality from the visual elements that will be exposed to the application.
 */

#pragma once
#include "Weekdays.h"
#include <SparkFunSX1509.h> // Include SX1509 library

//Common controls for all display elements
bool initVisualElements(SX1509* io1, SX1509*io2);
void setBrightness(byte brightness);
void setVisible(bool isVisible);
void redraw();

//Controls for the weekday LEDs
void showWeekDay(WEEKDAYS wd);

//Control of Alarm display
typedef enum
{
    AL_MINUTES_ONLY,
    AL_HOURS_ONLY,
    AL_BOTH_ON,
    AL_BOTH_OFF
}
ALARM_DISPLAY;
void showAlarmDisplay(ALARM_DISPLAY ad);
void showAlarmTime(byte hours, byte minutes);

//Controls for the LED array
void showClockTime(byte hours, byte minutes, bool synced, bool alarmOngoing);
void showSunlightSetting(byte value);
void showSongChoice(byte value);
void showSongVolume(byte value);
