#include "ButtonManager.h"

bool ButtonManager::addButton(ParameterPushButton *button)
{
    ParameterPushButton **moreButtons = (ParameterPushButton **)realloc(_buttonlist, ++_nrOfButtons * sizeof(ParameterPushButton *));
    if (moreButtons == nullptr)
    {
        free(moreButtons);
        return false;
    }
    _buttonlist = moreButtons;
    _buttonlist[_nrOfButtons - 1] = button;
    return true;
}

/**
 * \return  true when the key has been found in the list and it wasn't a double keypress
 */
bool ButtonManager::keyPressed(byte key)
{
    bool retVal = false;
    ParameterPushButton *newActiveButton = nullptr;
    for (byte i = 0; i < _nrOfButtons; i++)
    {
        if (_buttonlist[i]->key() == key && _buttonlist[i]->isEnabled())
        {
            newActiveButton = _buttonlist[i];
        }
    }
    if (_activeButton != nullptr)
    {
        //In case previous key was valid, disable it
        _activeButton->doAction(false);
    }
    if ((newActiveButton == _activeButton) || newActiveButton == nullptr)
    {
        //Same key pushed twice or no pushed key doesn't belong to this list ->  Disable functionality
        newActiveButton = nullptr;
        if (_rec != nullptr)
        {
            _rec->setConsumer(nullptr, false);
        }
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

void ButtonManager::attachRotaryEncoder(RotaryEncoderConsumer *rec)
{
    _rec = rec;
}
