#pragma once
#include "actions.h"
#include "Menu.h"

struct CommonConfig
{
    byte dayBright = 20;
    byte dayNight = 20;
    byte nightBright = 20;
};
extern CommonConfig config1;

struct CommonParameters
{
    FieldParameter dayBright = {0, &config1.dayBright, 100, 5, nullptr, nullptr};
    FieldParameter dayNight = {0, &config1.dayNight, 100, 5, nullptr, nullptr};
    FieldParameter nightBright = {0, &config1.nightBright, 100, 5, nullptr, nullptr};
};
extern CommonParameters compar;

struct AlarmConfig
{
    byte lightness = 50;
    byte volume = 50;
    byte song = 3;
    byte hours = 12;
    byte minutes = 15;
    bool weekdays[7] = {false, false, false, false, false, false, false};
};
extern AlarmConfig config2;

struct AlarmParameters
{
    FieldParameter lightness = {0, &config2.lightness, 100, 5, showLightness, stopLightness};
    FieldParameter volume = {0, &config2.volume, 100, 5, nullptr, nullptr};
    SelectParameter song = {&config2.song, 9, playSong, nullptr};
    FieldParameter hours = {0, &config2.hours, 23, 1, nullptr, nullptr};
    FieldParameter minutes = {0, &config2.minutes, 55, 5, nullptr, nullptr};
    bool* weekdays = config2.weekdays;
};
extern AlarmParameters alarms;
