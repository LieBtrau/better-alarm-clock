#pragma once
#include "actions.h"
#include "Menu.h"

struct CommonParameters
{
    FieldParameter dayBright = {0, 50, 100, 5, nullptr, nullptr};
    FieldParameter dayNight = {0, 50, 100, 5, nullptr, nullptr};
    FieldParameter nightBright = {0, 50, 100, 5, nullptr, nullptr};
};
extern CommonParameters compar;

struct AlarmParameters
{
    FieldParameter lightness = {0, 50, 100, 5, showLightness, stopLightness};
    FieldParameter volume = {0, 50, 100, 5, nullptr, nullptr};
    SelectParameter song = {3, 9, playSong, nullptr};
    FieldParameter hours = {0, 12, 23, 1, nullptr, nullptr};
    FieldParameter minutes = {0, 15, 55, 5, nullptr, nullptr};
    bool weekdays[7] = {false, false, false, false, false, false, false};
};
extern AlarmParameters alarms[2];
