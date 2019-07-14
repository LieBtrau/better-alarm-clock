#include "ButtonManager.h"

bool ButtonManager::addButton(PushButton *button)
{
    PushButton **moreButtons = (PushButton **)realloc(_buttonlist, ++_nrOfButtons * sizeof(PushButton *));
    if (moreButtons == nullptr)
    {
        free(moreButtons);
        return false;
    }
    _buttonlist = moreButtons;
    _buttonlist[_nrOfButtons - 1] = button;
    return true;
}

bool ButtonManager::render(bool forceRender)
{
    bool redrawNeeded = false;
    for (byte i = 0; i < _nrOfButtons; i++)
    {
        redrawNeeded |= _buttonlist[i]->render(forceRender);
    }
    return redrawNeeded;
}

bool ParameterButtonManager::render(bool forceRender)
{
    bool redrawNeeded = false;
    for (byte i = 0; i < _nrOfButtons; i++)
    {
        redrawNeeded |= _buttonlist[i]->render(forceRender);
        redrawNeeded |= ((ParameterPushButton *)_buttonlist[i])->getParam()->render(forceRender);
    }
    return redrawNeeded;
}

bool ParameterButtonManager::addButton(ParameterPushButton *button)
{
    return ButtonManager::addButton(button);
}

bool ToggleButtonManager::addButton(TogglePushButton *button)
{
    return ButtonManager::addButton(button);
}

/**
 * \return  true when the key has been found in the list and it wasn't a double keypress
 */
bool ParameterButtonManager::keyPressed(byte key)
{
    bool retVal = false;
    ParameterPushButton *newActiveButton = nullptr;
    for (byte i = 0; i < _nrOfButtons; i++)
    {
        if (_buttonlist[i]->key() == key && _buttonlist[i]->isEnabled())
        {
            newActiveButton = (ParameterPushButton *)_buttonlist[i];
        }
    }
    if (_activeButton != nullptr)
    {
        //In case previous key was valid, disable it
        ((ParameterPushButton *)_activeButton)->doAction(false);
    }
    if ((newActiveButton == _activeButton) || newActiveButton == nullptr)
    {
        //Same key pushed twice or no pushed key doesn't belong to this list ->  Disable functionality
        newActiveButton = nullptr;
    }
    else
    {
        //New key, execute it's action
        newActiveButton->doAction(true);
        if (_rec != nullptr)
        {
            _rec->setConsumer(newActiveButton->getParam(), false);
        }
        retVal = true;
    }
    _activeButton = newActiveButton;
    return retVal;
}



bool ToggleButtonManager::keyPressed(byte key)
{
    bool pressed = false;
    for (int i = 0; i < _nrOfButtons; i++)
    {
        if (_buttonlist[i]->key() == key && _buttonlist[i]->isEnabled())
        {
            ((TogglePushButton *)_buttonlist[i])->toggle();
            pressed = true;
        }
    }
    return pressed;
}

void ParameterButtonManager::attachRotaryEncoder(RotaryEncoderConsumer *rec)
{
    _rec = rec;
}

void ButtonManager::enable()
{
    for (byte i = 0; i < _nrOfButtons; i++)
    {
        _buttonlist[i]->enable();
    }
}

void ButtonManager::disable()
{
    for (byte i = 0; i < _nrOfButtons; i++)
    {
        _buttonlist[i]->disable();
    }
}

void ParameterButtonManager::enable()
{
    for (byte i = 0; i < _nrOfButtons; i++)
    {
        _buttonlist[i]->enable();
        ((ParameterPushButton *)_buttonlist[i])->getParam()->setVisible(true);
    }
}

void ParameterButtonManager::disable()
{
    for (byte i = 0; i < _nrOfButtons; i++)
    {
        _buttonlist[i]->disable();
        ((ParameterPushButton *)_buttonlist[i])->getParam()->setVisible(false);
    }
}
