#pragma once
#include "Max72xxPanel.h"         //32x16 LED matrix panel
#include "Chaplex.h"              //Charlieplexed LEDs
#include "Adafruit_LEDBackpack.h" //7segment display

struct Coordinate
{
    byte x = 0;
    byte y = 0;
};

struct FieldParameter
{
    byte min = 0;
    byte cur = 0;
    byte max = 0;
    byte step = 0;
};

typedef void (*voidFuncPtrByte)(byte b);
struct SelectParameter
{
    byte cur = 0;
    byte max = 10;
    voidFuncPtrByte doAction = nullptr;
};

class MenuOut
{
public:
    bool render(bool forceRender = false); //returns true if content has changed since last call, which would imply a redraw is needed.
    bool flash();
    virtual void hide() = 0;

protected:
    virtual void show() = 0;
    bool updateNeeded = true;

private:
    const unsigned long FLASH_INTERVAL = 500;
    bool visible = false;
    unsigned long ulTimer = millis();
};

class ParameterUpdate : public MenuOut
{
public:
    virtual bool increase() = 0;
    virtual bool decrease() = 0;
};

class Field : public ParameterUpdate
{
public:
    virtual bool increase();
    virtual bool decrease();
    Field(FieldParameter *par);

protected:
    FieldParameter *_val;
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
    void hide();

protected:
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
    PushButton(BUTTONS key, LedToggle *led);
    PushButton(BUTTONS key, LedToggle *led, ParameterUpdate *param);
    BUTTONS key();
    void setAction(voidFuncPtrBool doAction);
    void doAction(bool selected);
    ParameterUpdate *getParam();

private:
    BUTTONS _key;
    LedToggle *_led;
    voidFuncPtrBool _doAction = nullptr;
    ParameterUpdate *_param = nullptr;
};
