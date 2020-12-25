#pragma once

#include "Arduino.h"
#include "inputs.h"

void setWeekdayPointer(WEEKDAYS* pwd);
bool processWeekdayButtonEvent(KEY_CODE key);
void showWeek();
