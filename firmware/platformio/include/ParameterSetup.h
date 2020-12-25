#pragma once
#include "Tactiles.h"

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
