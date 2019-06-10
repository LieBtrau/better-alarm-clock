#include "Menu.h"

bool MenuOut::render()
{
    if(updateNeeded)
    {
        show();
    }
    bool retVal = updateNeeded;
    updateNeeded=false;
    return retVal;
}

PushButton::PushButton(BUTTONS key, LedToggle* led, voidFuncPtrBool doAction): _key(key), _led(led), _doAction(doAction){}

BUTTONS PushButton::key()
{
    return _key;
}

void PushButton::doAction(bool selected)
{
    if(selected)
    {
        _led->set();
    }else
    {
        _led->clear();
    }
    _doAction(selected);
}