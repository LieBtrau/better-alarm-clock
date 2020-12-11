#pragma once

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "AsyncDelay.h"
#include "Max72xxPanel.h"
#include <SparkFunSX1509.h>
#include "LedDriverDimming.h"

struct Coordinate
{
    byte x = 0;
    byte y = 0;
};

class MenuOut
{
public:
    MenuOut();
    bool render(bool forceRender = false);
    void setFlashMode(bool isFlashing);
    void setVisible(bool isVisible);

protected:
    virtual void show() = 0;  //routine that will write peripheral to show data
    virtual void hide() = 0;  //routine that will write peripheral to hide data
    bool updateNeeded = true; //IO-hardware must be written to update the visuals, introduced so that there would be only hardware call for all visuals belonging to the same peripheral.

private:
    const unsigned long FLASH_INTERVAL = 500;
    bool visible = false;
    bool _flashMode = false;
    bool flashVisible = true;
    AsyncDelay _flashTimer;
};

class SevenSegmentField : public MenuOut
{
public:
    SevenSegmentField(Adafruit_7segment *panel, byte leftPos);
    void setValue(byte data);
    static const byte LEFTPOS = 0;
    static const byte RIGHTPOS = 3;

protected:
    virtual void show();
    virtual void hide();

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
    void setValidity(bool timeIsValid);

protected:
    virtual void show();
    virtual void hide();

private:
    Max72xxPanel *_panel;
    byte _hours = 0;
    byte _minutes = 0;
    bool _synced = false;
    bool _timeIsValid=false;
};

class LedMatrixField : public MenuOut
{
public:
    LedMatrixField(Max72xxPanel *panel, Coordinate topleft, Coordinate botRight, byte max);
    void setValue(byte cur);

protected:
    virtual void show();
    virtual void hide();

private:
    Max72xxPanel *_panel;
    Coordinate _topleft;
    Coordinate _botRight;
    byte _max;
    byte _cur;
};

class LedMatrixSelect : public MenuOut
{
public:
    LedMatrixSelect(Max72xxPanel *panel, Coordinate topleft, Coordinate botRight, byte max);
    void setValue(byte cur);

protected:
    virtual void show();
    virtual void hide();

private:
    Max72xxPanel *_panel;
    Coordinate _topleft;
    Coordinate _botRight;
    byte _max;
    byte _cur;
};

class LedToggle : public MenuOut
{
public:
    LedToggle(SX1509* io, byte pinNr);
    void init();
    void setBrightness(byte value);

protected:
    virtual void show();
    virtual void hide();

private:
    SX1509* _io;
    byte _pinNr;
    byte _brightness;
};

class SunRiseEmulation : public MenuOut
{
public:
    SunRiseEmulation(LedDriverDimming* led);
    void setBrightness(float value);

protected:
    virtual void show();
    virtual void hide();

private:
    LedDriverDimming* _led;
    float _brightness;
};

