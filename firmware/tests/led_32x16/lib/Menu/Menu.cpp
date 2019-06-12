#include "Menu.h"

bool MenuOut::render()
{
    if (updateNeeded)
    {
        show();
    }
    bool retVal = updateNeeded;
    updateNeeded = false;
    return retVal;
}

PushButton::PushButton(BUTTONS key, LedToggle *led, ParameterUpdate *param) : _key(key), _led(led), _param(param) {}

BUTTONS PushButton::key()
{
    return _key;
}

void PushButton::doAction(bool selected)
{
    if (selected)
    {
        _led->set();
    }
    else
    {
        _led->clear();
    }
    if (_doAction != nullptr)
    {
        _doAction(selected);
    }
}

ParameterUpdate* PushButton::getParam()
{
    return _param;
}