#include "Menu.h"

LedToggle::LedToggle(Chaplex *chappy, CharlieLed *led, bool* value) : _chappy(chappy), _led(led), _val(value)
{
}

void LedToggle::render()
{
    _chappy->setLedState(_led, *_val);
}

void LedToggle::hide()
{
    _chappy->setLedState(_led, false);
}

void LedToggle::set()
{
    *_val = true;
}

void LedToggle::clear()
{
    *_val = false;
}
