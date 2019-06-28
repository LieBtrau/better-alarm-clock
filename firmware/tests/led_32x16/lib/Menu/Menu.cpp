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

void ParameterUpdate::setLinkedParameter(ParameterUpdate *linkedP)
{
    _linkedP = linkedP;
}

ParameterUpdate *ParameterUpdate::getLinkedParameter()
{
    return _linkedP;
}

PushButton::PushButton(BUTTONS keyStroke, LedToggle* led): _key(keyStroke), _led(led){}
TogglePushButton::TogglePushButton(BUTTONS key, LedToggle *led) : PushButton(key, led) {}
ActionPushButton::ActionPushButton(BUTTONS key, LedToggle *led) : PushButton(key, led) {}
ParameterPushButton::ParameterPushButton(BUTTONS key, LedToggle *led, ParameterUpdate *param) : ActionPushButton(key, led), _param(param) {}

BUTTONS PushButton::key()
{
    return _key;
}

void TogglePushButton::toggle()
{
    _led->toggle();
}

void ActionPushButton::doAction(bool selected)
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

void ActionPushButton::setAction(voidFuncPtrBool doAction)
{
    _doAction = doAction;
}

ParameterUpdate *ParameterPushButton::getParam()
{
    return _param;
}