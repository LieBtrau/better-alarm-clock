#pragma once
#include "Arduino.h"
#include "alarmcalendar.h"

struct CommonConfig
{
    byte dayBright = 7;
    byte dayNight = 4;
    byte nightBright = 7;
};

struct AlarmConfig
{
    byte lightness = 50;
    byte volume = 10;
    byte song = 3;
    ALARM_CONFIG time;
};

typedef enum
{
    ALARM1,
    ALARM2,
    MAX_ALARMS
}ALARMNRS;