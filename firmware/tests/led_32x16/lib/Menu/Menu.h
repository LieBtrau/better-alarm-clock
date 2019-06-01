#pragma once
#include "Max72xxPanel.h"

class Menu
{
public:
    virtual void render()=0;
};

struct Coordinate
{
    byte x=0;
    byte y=0;
};

struct FieldParameter
{
    byte min=0;
    byte cur=0;
    byte max=0;
    byte step=0;
};

typedef void (*voidFuncPtrByte)(byte b);
struct SelectParameter
{
    byte cur=0;
    byte max=10;
    voidFuncPtrByte doAction = nullptr;
};



class LedMatrixField : public Menu
{
public:
    LedMatrixField(Max72xxPanel* panel, Coordinate topleft, Coordinate botRight, FieldParameter* par);
    void render();
    bool increase();
    bool decrease();
private:
    Max72xxPanel* _panel;
    Coordinate _topleft;
    Coordinate _botRight;
    FieldParameter* _val;
};



class LedMatrixSelect : public Menu
{
public:
    LedMatrixSelect(Max72xxPanel* panel, Coordinate topleft, Coordinate botRight, SelectParameter* par);
    void render();
    void next();
    void prev();
private:
    Max72xxPanel* _panel;
    Coordinate _topleft;
    Coordinate _botRight;
    SelectParameter* _par;
};