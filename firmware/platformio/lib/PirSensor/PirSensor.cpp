#include "PirSensor.h"

PirSensor::PirSensor(byte pin) : _pirPin(pin)
{
}

void PirSensor::init()
{
    pinMode(_pirPin, INPUT_PULLUP);
}

bool PirSensor::movementDetected()
{
    return digitalRead(_pirPin) ? true : false;
}