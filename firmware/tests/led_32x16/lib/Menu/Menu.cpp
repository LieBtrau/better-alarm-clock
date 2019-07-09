#include "Menu.h"

/* forceRender can be used to force a show() when the underlying parameter hasn't changed.  This behavior is useful in the case of flashing.
 * returns true if content has changed since last call, which would imply a redraw is needed.
 */
bool MenuOut::render(bool forceRender)
{
    if (visible && (updateNeeded || forceRender))
    {
        show();
    }
    bool retVal = updateNeeded;
    updateNeeded = false;
    return retVal;
}

void MenuOut::setVisible(bool isVisible)
{
    visible = isVisible;
    if (!visible)
    {
        hide();
    }
    else
    {
        show();
    }
    updateNeeded = true;
}

bool MenuOut::flash()
{
    if (millis() < ulTimer + FLASH_INTERVAL)
    {
        return false;
    }
    ulTimer = millis();
    if (flashVisible)
    {
        hide();
    }
    else
    {
        render(true);
    }
    flashVisible = !flashVisible;
    return true;
}

ClockFace::ClockFace(voidFuncPtrClockTime showFunction, voidFuncPtrVoid hideFunction) : _drawFunction(showFunction), _hideFunction(hideFunction) {}

void ClockFace::setTime(byte hours, byte minutes)
{
    if (hours != _hours || minutes != _mins)
    {
        updateNeeded = true;
    }
    _hours = hours;
    _mins = minutes;
}

void ClockFace::show()
{
    hide();
    _drawFunction({_hours, _mins});
}

void ClockFace::hide()
{
    _hideFunction();
}

void ParameterUpdate::setLinkedParameter(ParameterUpdate *linkedP)
{
    _linkedP = linkedP;
}

ParameterUpdate *ParameterUpdate::getLinkedParameter()
{
    return _linkedP;
}

PushButton::PushButton(BUTTONS keyStroke, LedToggle *led) : _key(keyStroke), _led(led) {}
TogglePushButton::TogglePushButton(BUTTONS key, LedToggle *led) : PushButton(key, led) {}
ActionPushButton::ActionPushButton(BUTTONS key, LedToggle *led) : PushButton(key, led) {}
ParameterPushButton::ParameterPushButton(BUTTONS key, LedToggle *led, ParameterUpdate *param) : ActionPushButton(key, led), _param(param) {}

BUTTONS PushButton::key()
{
    return _key;
}

void PushButton::enable()
{
    _isEnabled = true;
}

void PushButton::disable()
{
    _isEnabled = false;
}

bool PushButton::isEnabled()
{
    return _isEnabled;
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