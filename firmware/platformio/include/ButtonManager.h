#pragma once
#include "Menu.h"
#include "rotaryEncoder.h"

class ButtonManager
{
public:
    virtual bool keyPressed(byte key) = 0;
    virtual void enable();
    virtual void disable();
    virtual bool render(bool forceRender=false);

protected:
    bool addButton(PushButton *button);
    byte _nrOfButtons = 0;
    PushButton *_activeButton = nullptr;
    PushButton **_buttonlist = nullptr;
};

class ParameterButtonManager : public ButtonManager
{
public:
    virtual bool keyPressed(byte key);
    virtual void enable();
    virtual void disable();
    virtual bool render(bool forceRender=false);
    void attachRotaryEncoder(RotaryEncoderConsumer *rec);
    bool addButton(ParameterPushButton *button);
private:
    RotaryEncoderConsumer *_rec = nullptr;
};

class ToggleButtonManager : public ButtonManager
{
public:
    virtual bool keyPressed(byte key);
    bool addButton(TogglePushButton *button);
    
private:
};
