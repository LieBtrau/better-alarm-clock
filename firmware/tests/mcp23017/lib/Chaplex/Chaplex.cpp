/*
||
|| @author Stefan Götze 
||         using Alexander Brevig - Charlieplex library
|| @version 0.1
||
|| Changes by Christoph Tack:
    - reducing memory footprint
    - storing states in a byte instead of byte array.  That allows for 8x7 LEDs.
        If you want more, change the type to uint16 or uint32.
*/

#include "Chaplex.h"

Chaplex::Chaplex()
{
    allClear();
}

// set control-array for one LED ON/OFF
bool Chaplex::setLedState(CharlieLed* led)
{
    if ((led->a < 5) && (led->c < 5) && (led->a != led->c))
    {
        bitWrite(ledCtrl[led->a], led->c, led->on);
        return true;
    }
    return false;
}

//set control-array for all LEDs OFF
void Chaplex::allClear()
{
    memset(ledCtrl, 0, 5);
}

// Turn a single LED ON at full brightness or OFF.  All other LEDs will be turned off as well.
void Chaplex::setSingleLed(CharlieLed* led)
{ /*
    if(!setLedState(led, state))
    {
        return;
    }
    for (byte i=0; i<numberOfPins; i++)
    {
        pPinmode(pins[i], i==led.a || i==led.c ? OUTPUT:INPUT);
    }
    pDigitalWrite(pins[led.a], HIGH);
    pDigitalWrite(pins[led.c], LOW);*/
}

// Write led states to pins.  Needs to be called regularly for persistance of vision.
bool Chaplex::showLedState(byte &pinModes, byte &gpioStates)
{
    byte pinModesIn = pinModes;
    byte gpioStatesIn = gpioStates;
    if (++ledRow >= 5)
    {
        ledRow = 0;
    }
    byte curRow = ledCtrl[ledRow];
    pinModes |=0x1F;            //Set all lines as input
    pinModes &= ~curRow;        //Set needed lines as output
    bitClear(pinModes, ledRow); // current line is output
    gpioStates &= 0xE0;         // make all IO lines low <-- HARD CODED for 5 data lines!!
    bitSet(gpioStates, ledRow); // current line high
    
    return ((pinModes != pinModesIn) || (gpioStates!=gpioStatesIn));
}
