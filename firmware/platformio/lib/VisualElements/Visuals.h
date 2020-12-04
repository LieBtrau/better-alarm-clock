#pragma once

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "AsyncDelay.h"
#include "Max72xxPanel.h"

class MenuOut
{
public:
    MenuOut();
    bool render(bool forceRender = false);
    bool flash();
    void setVisible(bool isVisible);

protected:
    virtual void show() = 0;  //routine that will write peripheral to show data
    bool updateNeeded = true; //IO-hardware must be written to update the visuals, introduced so that there would be only hardware call for all visuals belonging to the same peripheral.
    virtual void hide() = 0;  //routine that will write peripheral to hide data

private:
    const unsigned long FLASH_INTERVAL = 500;
    bool visible = false;
    bool flashVisible = true;
    AsyncDelay _flashTimer;
};

class SevenSegmentField : public MenuOut
{
public:
    SevenSegmentField(Adafruit_7segment *panel);
    bool init(byte leftPos);
    void setValue(byte data);
    virtual void hide();
    static const byte LEFTPOS = 0;
    static const byte RIGHTPOS = 3;

protected:
    virtual void show();

private:
    Adafruit_7segment *_panel;
    byte _leftPos = 0;
    byte _data = 0;
};

class ClockFace : public MenuOut
{
public:
    ClockFace(Max72xxPanel *panel);
    void setTime(byte hours, byte minutes, bool synced);

protected:
    virtual void show();
    virtual void hide();

private:
    Max72xxPanel *_panel;
    byte _hours = 0;
    byte _minutes = 0;
    bool _synced = false;
};
