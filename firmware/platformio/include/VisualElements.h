/* This class contains functionality from the visual elements that will be exposed to the application.
 */

#pragma once

//Common controls for all display elements
bool initVisualElements();
void setBrightness(byte brightness);
void redraw();

//Controls for the weekday LEDs
typedef enum
{
    WD_MONDAY = 0x01,
    WD_TUESDAY = 0x02,
    WD_WEDNESDAY = 0x04,
    WD_THURSDAY = 0x08,
    WD_FRIDAY = 0x10,
    WD_SATURDAY = 0x20,
    WD_SUNDAY = 0x40
}
WEEKDAYS;
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
void showTime(byte hours, byte minutes, bool synced, bool alarmOngoing);
void showSunlightSetting(byte value);
void showSongChoice(byte value);
void showSongVolume(byte value);
