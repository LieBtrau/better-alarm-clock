#include "Menu.h"
#include "fontBig.h"

MenuOut::MenuOut()
{
    flashTimer.start(FLASH_INTERVAL);
}

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
    if ((!isVisible) && (!visible))
    {
        return;
    }
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
    if (!flashTimer.justFinished())
    {
        return false;
    }
    flashTimer.restart();
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

ClockFace::ClockFace(Max72xxPanel *panel) : _panel(panel) {}

void ClockFace::setTime(ClockTime time)
{
    if (_time.hours != time.hours || _time.mins != time.mins)
    {
        updateNeeded = true;
    }
    _time=time;
}

void ClockFace::show()
{
    hide();
    byte font = 4; //0 to 4
    _panel->drawBitmap(-3, 1, bigFont[(_time.hours / 10 != 0 ? _time.hours / 10 : 10) + 11 * font], 8, 12, 1);
    _panel->drawBitmap(5, 1, bigFont[(_time.hours % 10) + 11 * font], 8, 12, 1);
    _panel->drawBitmap(16, 1, bigFont[(_time.mins / 10 != 0 ? _time.mins / 10 : 10) + 11 * font], 8, 12, 1);
    _panel->drawBitmap(24, 1, bigFont[(_time.mins % 10) + 11 * font], 8, 12, 1);
    if (_time.synced)
    {
        _panel->fillRect(15, 4, 2, 2, 1);
        _panel->fillRect(15, 10, 2, 2, 1);
    }
    else
    {
        _panel->fillRect(15, 7, 2, 2, 1);
    }
}

void ClockFace::hide()
{
    _panel->fillScreen(0);
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
    _led->setVisible(true);
}

void PushButton::disable()
{
    _isEnabled = false;
    _led->setVisible(false);
}

bool PushButton::isEnabled()
{
    return _isEnabled;
}

bool PushButton::render(bool forceRender)
{
    return _led->render(forceRender);
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

void ActionPushButton::setAction(voidFuncPtrBool doAction)
{
    _doAction = doAction;
}

ParameterUpdate *ParameterPushButton::getParam()
{
    return _param;
}