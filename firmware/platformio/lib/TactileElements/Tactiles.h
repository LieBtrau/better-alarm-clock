#pragma once

#include <RotaryEncoder.h>
#include <SparkFunSX1509.h> // Include SX1509 library
#include "AsyncDelay.h"

class Tactiles
{
public:
    virtual bool isChanged() = 0;
    virtual int getValue() = 0;

protected:
private:
};

class RotaryEncoder_Tactiles : Tactiles
{
public:
    RotaryEncoder_Tactiles(byte pinData, byte pinClock);
    virtual bool isChanged();
    virtual int getValue();

private:
    RotaryEncoder _re; //Fully encapsulate rotary encoder in this class, so that we can control pinData & pinClock
    int _lastPosition = 0;
};

class Sx1509_Tactiles : Tactiles
{
public:
    Sx1509_Tactiles(SX1509 *sx, byte pinIrq);
    void init(byte keyRows, byte keyCols);
    virtual bool isChanged();
    virtual int getValue();
    void processIrq();

private:
    SX1509 *_sx;
    byte _pin_irq;
    int _previousKeyData = 0;
    bool _newKey = false;
    AsyncDelay _keyReleaseTimeout;
};

//The Bounce2 library could have been used, but it requires polling the gpio status.  I prefer interrupts.
class Gpio_Tactiles : Tactiles
{
public:
    Gpio_Tactiles(byte pinGpio, int pin_mode);
    ~Gpio_Tactiles();
    virtual bool isChanged();
    virtual int getValue();
private:
    byte _pinGpio=0;
    int _lastGpioState=0;
};