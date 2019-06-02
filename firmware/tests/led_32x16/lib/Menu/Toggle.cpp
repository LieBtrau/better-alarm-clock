#include "Menu.h"

LedToggle::LedToggle(Chaplex *chappy, CharlieLed *led) : _chappy(chappy), _led(led)
{
}

void LedToggle::render()
{
    _chappy->setLedState(_led);
}

void LedToggle::set()
{
    _led->on = true;
}

void LedToggle::clear()
{
    _led->on = false;
}
