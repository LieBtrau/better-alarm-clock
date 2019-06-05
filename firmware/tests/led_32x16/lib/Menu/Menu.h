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
    bool render(); //returns true if content has changed since last call, which would imply a redraw is needed.
    virtual void hide() = 0;

protected:
    virtual void show() = 0;
    bool updateNeeded = true;
};

class Field : public MenuOut
{
public:
    Field(FieldParameter *par);
    bool increase();
    bool decrease();

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

class LedMatrixSelect : public MenuOut
{
public:
    LedMatrixSelect(Max72xxPanel *panel, Coordinate topleft, Coordinate botRight, SelectParameter *par);
    void next();
    void prev();
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
    void hide();

protected:
    virtual void show();

private:
    Chaplex *_chappy;
    CharlieLed *_led;
    bool *_val;
};