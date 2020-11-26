#pragma once
#include "Arduino.h"

class PirSensor
{
public:
    PirSensor(byte pin);
    void init();
    bool movementDetected();
private:
    byte _pirPin;
};