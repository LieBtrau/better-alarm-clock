#pragma once
#include "Arduino.h"

bool initDisplayOnOffControl();
bool displayTurnedOn(bool buttonPressed);
bool isDisplayOn(bool buttonPressed);
bool getDisplayBrightness(byte &brightness);