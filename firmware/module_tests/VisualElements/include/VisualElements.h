#pragma once

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

typedef enum
{
    AL_MINUTES_ONLY,
    AL_HOURS_ONLY,
    AL_BOTH_ON,
    AL_BOTH_OFF
}
ALARM_DISPLAY;

bool initVisualElements();
void setBrightness(byte brightness);
void redraw();
void showTime(byte hours, byte minutes, bool synced);
void showWeekDay(WEEKDAYS wd);
void showAlarmDisplay(ALARM_DISPLAY ad);