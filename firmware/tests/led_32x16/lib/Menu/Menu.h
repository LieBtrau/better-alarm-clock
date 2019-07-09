#pragma once
#include "Max72xxPanel.h"         //32x16 LED matrix panel
#include "Chaplex.h"              //Charlieplexed LEDs
#include "Adafruit_LEDBackpack.h" //7segment display

struct Coordinate
{
    byte x = 0;
    byte y = 0;
};

struct ClockTime
{
    byte hours = 0;
    byte mins = 0;
};

typedef void (*voidFuncPtrByte)(byte b);
typedef void (*voidFuncPtrVoid)(void);
struct FieldParameter
{
    byte min = 0;
    byte *cur = nullptr;
    byte max = 0;
    byte step = 0;
    voidFuncPtrByte doAction = nullptr;
    voidFuncPtrVoid stopAction = nullptr;
};

struct SelectParameter
{
    byte *cur = nullptr;
    byte min = 1;
    byte max = 10;
    voidFuncPtrByte doAction = nullptr;
    voidFuncPtrVoid stopAction = nullptr;
};

class MenuOut
{
public:
    bool render(bool forceRender = false);
    bool flash();
    void setVisible(bool isVisible);

protected:
    virtual void show() = 0;
    bool updateNeeded = true; //IO-hardware must be written to update the visuals
    virtual void hide() = 0;

private:
    const unsigned long FLASH_INTERVAL = 500;
    bool visible = true;
    bool flashVisible = true;
    unsigned long ulTimer = millis();
};

typedef void (*voidFuncPtrClockTime)(ClockTime ct);
class ClockFace : public MenuOut
{
public:
    ClockFace(voidFuncPtrClockTime showFunction, voidFuncPtrVoid hideFunction);
    void setTime(byte hours, byte minutes);

protected:
    virtual void show();
    virtual void hide();

private:
    byte _hours;
    byte _mins;
    voidFuncPtrClockTime _drawFunction = nullptr;
    voidFuncPtrVoid _hideFunction = nullptr;
};

class ParameterUpdate : public MenuOut
{
public:
    virtual bool increase() = 0;
    virtual bool decrease() = 0;
    virtual void stopAction() = 0;
    void setLinkedParameter(ParameterUpdate *linkedP); //so that music can keep playing when switching from song selection to volume control
    ParameterUpdate *getLinkedParameter();
    virtual void doAction() = 0;

private:
    ParameterUpdate *_linkedP = nullptr;
};

class Field : public ParameterUpdate
{
public:
    virtual bool increase();
    virtual bool decrease();
    Field(FieldParameter *par);
    virtual void stopAction();
    virtual void doAction();

protected:
    FieldParameter *_par;
};

class LedMatrixField : public Field
{
public:
    LedMatrixField(Max72xxPanel *panel, Coordinate topleft, Coordinate botRight, FieldParameter *par);
    virtual void hide();

protected:
    virtual void show();

private:
    Max72xxPanel *_panel;
    Coordinate _topleft;
    Coordinate _botRight;
};

class SevenSegmentField : public Field
{
public:
    SevenSegmentField(Adafruit_7segment *panel, byte leftPos, FieldParameter *par);
    virtual void hide();
    static const byte LEFTPOS = 0;
    static const byte RIGHTPOS = 3;

protected:
    virtual void show();

private:
    Adafruit_7segment *_panel;
    byte _leftPos;
};

class LedMatrixSelect : public ParameterUpdate
{
public:
    LedMatrixSelect(Max72xxPanel *panel, Coordinate topleft, Coordinate botRight, SelectParameter *par);
    virtual bool increase();
    virtual bool decrease();
    virtual void hide();
    virtual void stopAction();

protected:
    virtual void doAction();
    virtual void show();

private:
    Max72xxPanel *_panel;
    Coordinate _topleft;
    Coordinate _botRight;
    SelectParameter *_par;
};

class LedToggle : public MenuOut
{
public:
    LedToggle(Chaplex *chappy, CharlieLed *led, bool *value);
    void set();
    void clear();
    void toggle();
    void hide();
    bool isLedOn();
    void setSource(bool *val);

protected:
    virtual void show();

private:
    Chaplex *_chappy;
    CharlieLed *_led;
    bool *_val;
};

//---------------------------------------------------------------------------------------------------------------------------
enum BUTTONS
{
    LIGHTNESS = 3,
    VOLUME = 2,
    SONGCHOICE = 1,
    ALARMTIME = 0,
    MENU = 6,
    MONDAY = 8,
    TUESDAY = 9,
    WEDNESDAY = 10,
    THURSDAY = 11,
    FRIDAY = 12,
    SATURDAY = 13,
    SUNDAY = 14,
    NIGHTDISPLAYBRIGHTNESS = 5,
    DAYNIGHTLEVEL = 4,
    DAYDISPLAYBRIGHTNESS = 7
};

typedef void (*voidFuncPtrBool)(bool);

class PushButton
{
public:
    PushButton(BUTTONS keyStroke, LedToggle *led);
    BUTTONS key();
    void enable();
    void disable();
    bool isEnabled();

protected:
    BUTTONS _key;
    LedToggle *_led;

private:
    bool _isEnabled = true;
};

class TogglePushButton : public PushButton
{
public:
    TogglePushButton(BUTTONS key, LedToggle *led);
    void toggle();
};

class ActionPushButton : public PushButton
{
public:
    ActionPushButton(BUTTONS key, LedToggle *led);
    void setAction(voidFuncPtrBool doAction);
    void doAction(bool selected);

private:
    voidFuncPtrBool _doAction = nullptr;
};

class ParameterPushButton : public ActionPushButton
{
public:
    ParameterPushButton(BUTTONS key, LedToggle *led, ParameterUpdate *param);
    ParameterUpdate *getParam();

private:
    ParameterUpdate *_param = nullptr;
};
