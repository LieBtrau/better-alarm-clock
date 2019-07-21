#pragma once
#include "Arduino.h"

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
    byte hours = 12;
    byte minutes = 15;
    bool weekdays[7] = {false, false, false, false, false, false, true};
};
