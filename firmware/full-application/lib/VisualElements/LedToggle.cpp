#include "Visuals.h"

LedToggle::LedToggle(byte pinNr) : _pinNr(pinNr)
{
}

void LedToggle::init(SX1509 *io)
{
    _io = io;
    _io->pinMode(_pinNr, ANALOG_OUTPUT);
}

//\param brightness value 0 to 255
void LedToggle::setBrightness(byte brightness)
{
    _brightness = brightness;
    updateNeeded = true;
}

void LedToggle::show()
{
    _io->analogWrite(_pinNr, _brightness);
}

void LedToggle::hide()
{
    _io->analogWrite(_pinNr, 0);
}
