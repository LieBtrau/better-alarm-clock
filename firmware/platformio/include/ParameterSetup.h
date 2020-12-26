#pragma once
#include "Tactiles.h"

void disableParameterSetting();
void initPeripherals();
void showSunriseSetting(float sunRiseSetting);
int getTrackCount();
void playMusic(int track, int volume);

const int MAX_VOLUME = 30;
const int SETUP_TIMEOUT = 30000;    // period during which parameters can be adjusted (e.g. for )

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
