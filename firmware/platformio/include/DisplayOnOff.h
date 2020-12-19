#pragma once
#include "Arduino.h"

typedef enum
{
    DISPLAY_OFF,
    DISPLAY_TURNED_ON,
    DISPLAY_ON,
    DISPLAY_TURNED_OFF
} DISPLAY_STATE;

bool initDisplayOnOffControl();
DISPLAY_STATE getDisplayState(bool buttonPressed, bool newAlarmOngoing);
byte getDisplayBrightness(bool newAlarmOngoing);