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
};

class Chaplex
{

public:
    Chaplex();
    bool setLedState(CharlieLed *led, bool on);
    void allClear();
    bool showLedState(byte &pinModes, byte &gpioStates);
    void setSingleLed(CharlieLed *led, bool on);

private:
    byte ledCtrl[5];
    byte ledRow = 0;
    byte changedMask=0x00;  //helper to keep track of recently changed led rows
};
