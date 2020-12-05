#include "Visuals.h"

SunRiseEmulation::SunRiseEmulation(LedDriverDimming *led) : _led(led) {}

void SunRiseEmulation::setBrightness(float value)
{
    _brightness = value;
    updateNeeded = true;
}

void SunRiseEmulation::show()
{
    _led->setBrightness(_brightness);
    _led->enable(true);
}

void SunRiseEmulation::hide()
{
    _led->enable(false);
}
