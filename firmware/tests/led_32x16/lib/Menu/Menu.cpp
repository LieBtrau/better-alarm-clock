#include "Menu.h"

bool MenuOut::render(bool forceRender)
{
    if (updateNeeded || forceRender)
    {
        show();
    }
    bool retVal = updateNeeded;
    updateNeeded = false;
    return retVal;
}

bool MenuOut::flash()
{
    if (millis() < ulTimer + FLASH_INTERVAL)
    {
        return false;
    }
    ulTimer = millis();
    if (visible)
    {
        visible = false;
        hide();
    }
    else
    {
        visible = true;
        render(true);
    }
    return true;
}

PushButton::PushButton(BUTTONS key, LedToggle *led) : _key(key), _led(led) {}
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
        if (_doAction != nullptr)
        {
            _doAction(selected);
        }
    }
    else
    {
        _led->clear();
    }
}

void PushButton::setAction(voidFuncPtrBool doAction)
{
    _doAction = doAction;
}

ParameterUpdate *PushButton::getParam()
{
    return _param;
}