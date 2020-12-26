#pragma once
#include "Tactiles.h"
#include "ParameterSetup.h"

typedef struct
{
  WEEKDAYS wd = (WEEKDAYS)(WD_MONDAY | WD_TUESDAY | WD_WEDNESDAY | WD_THURSDAY | WD_FRIDAY);
  float sunRiseSetting = 50.0;
  int volume = 10;
  int songchoice = 12;
  int alarmHours = 7;
  int alarmMinutes = 0;
} CFG;//some byte in eeprom could be saved by using bytes instead.

void disableParameterSetting(CFG* config);
void initPeripherals(CFG* config);
void showSunriseSetting(float sunRiseSetting);
int getTrackCount();
void playMusic(int track, int volume);

const int MAX_VOLUME = 30;
const int SETUP_TIMEOUT = 5000;    // period during which parameters can be adjusted (e.g. for )

template <typename T>
byte rescaleParameter(T *parameter, T paramMin, T paramMax, byte maxVisualScale)
{
    return *parameter * maxVisualScale / (paramMax - paramMin);
}

template <typename T>
void updateParameter(T *parameter, T paramMin, T paramMax, T stepSize, RotaryEncoder_Tactiles::DIRECTION dir)
{
    switch (dir)
    {
    case RotaryEncoder_Tactiles::TURN_LEFT:
        if (*parameter - stepSize > paramMin)
        {
            *parameter -= stepSize;
        }
        else
        {
            *parameter = paramMin;
        }
        break;
    case RotaryEncoder_Tactiles::TURN_RIGHT:
        if (*parameter + stepSize < paramMax)
        {
            *parameter += stepSize;
        }
        else
        {
            *parameter = paramMax;
        }
    default:
        break;
    }
}
