#pragma once
#include "Max72xxPanel.h"   //32x16 LED matrix panel
#include "Chaplex.h"        //Charlieplexed LEDs

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
    virtual void render() = 0;
    virtual void hide() = 0;
};

class Field : public MenuOut
{
public:
    Field(FieldParameter* par);
    bool increase();
    bool decrease();
protected:
    FieldParameter *_val;
};

class LedMatrixField : public Field
{
public:
    LedMatrixField(Max72xxPanel *panel, Coordinate topleft, Coordinate botRight, FieldParameter *par);
    void render();
    void hide();

private:
    Max72xxPanel *_panel;
    Coordinate _topleft;
    Coordinate _botRight;
};

class LedMatrixSelect : public MenuOut
{
public:
    LedMatrixSelect(Max72xxPanel *panel, Coordinate topleft, Coordinate botRight, SelectParameter *par);
    void render();
    void next();
    void prev();
    void hide();

private:
    Max72xxPanel *_panel;
    Coordinate _topleft;
    Coordinate _botRight;
    SelectParameter *_par;
};

class LedToggle : public MenuOut
{
public:
    LedToggle(Chaplex* chappy, CharlieLed* led, bool* value);
    void render();
    void set();
    void clear();
    void hide();
private:
    Chaplex* _chappy;
    CharlieLed* _led;
    bool* _val;
};