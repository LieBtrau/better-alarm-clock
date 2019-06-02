/*
||
|| @author Stefan Götze 
||         using Alexander Brevig - Charlieplex library
|| @version 0.1
||
|| Updates by Christoph Tack
||
*/

#pragma once
#include "Arduino.h"

struct CharlieLed
{
    byte a;  // anode (corresponding bit in GPIO register)
    byte c;  // cathode (corresponding bit in GPIO register)
    bool on; // true = LED on, false = LED off
};

class Chaplex
{

public:
    Chaplex();
    bool setLedState(CharlieLed *led);
    void allClear();
    bool showLedState(byte &pinModes, byte &gpioStates);
    void setSingleLed(CharlieLed *led);

private:
    byte ledCtrl[5];
    byte ledRow = 0;
};
