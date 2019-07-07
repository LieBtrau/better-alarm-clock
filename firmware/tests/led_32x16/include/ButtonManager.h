#pragma once
#include "Menu.h"
#include "rotaryEncoder.h"

class ButtonManager
{
public:
    bool addButton(ParameterPushButton *button);
    bool keyPressed(byte key);
    void attachRotaryEncoder(RotaryEncoderConsumer *rec);

private:
    byte _prevKey = 0xFF;
    byte _nrOfButtons = 0;
    ParameterPushButton *_activeButton = nullptr;
    ParameterPushButton **_buttonlist = nullptr;
    RotaryEncoderConsumer *_rec = nullptr;
};