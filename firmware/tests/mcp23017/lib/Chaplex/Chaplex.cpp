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

Chaplex::Chaplex(byte* userPins, byte nrOfPins): numberOfPins(nrOfPins)
{
    pins = (byte*)malloc(sizeof(byte) * numberOfPins);
    memcpy(pins, userPins, nrOfPins);
    ledCtrl = (byte *)malloc(sizeof(byte) * numberOfPins);
    allClear();
    showLedState();
}

Chaplex::~Chaplex()
{
   if (pins)
   {
      free(pins);
   }
   if(ledCtrl)
   {
       free(ledCtrl);
   }
}

void Chaplex::setPinmode(voidFuncPtrUint32Uint32 function)
{
    pPinmode = function;
}

void Chaplex::setDigitalWrite(voidFuncPtrUint32Uint32 function)
{
    pDigitalWrite = function;
}

// set control-array for one LED ON/OFF
bool Chaplex::setLedState(charlieLed led, LEDSTATE state)
{
    if ((led.a<numberOfPins) && (led.c<numberOfPins) && (led.a!=led.c))
    {
        bitWrite(ledCtrl[led.a],led.c, state);
        return true;
    }
    return false;
}

//set control-array for all LEDs OFF
void Chaplex::allClear()
{
    memset(ledCtrl, OFF, numberOfPins);
}

// Turn a single LED ON at full brightness or OFF.  All other LEDs will be turned off as well.
void Chaplex::setSingleLed(charlieLed led, LEDSTATE state)
{
    if(!setLedState(led, state))
    {
        return;
    }
    for (byte i=0; i<numberOfPins; i++)
    {
        pPinmode(pins[i], i==led.a || i==led.c ? OUTPUT:INPUT);
    }
    pDigitalWrite(pins[led.a], HIGH);
    pDigitalWrite(pins[led.c], LOW);
}


// Write led states to pins.  Needs to be called regularly for persistance of vision.
void Chaplex::showLedState()
{
    pPinmode(pins[ledRow==0 ? numberOfPins-1 : ledRow-1],INPUT);
    for (byte i=0; i<numberOfPins; i++)
    {
        if(i!=ledRow)
        {
            pPinmode(pins[i],bitRead(ledCtrl[ledRow],i) ? OUTPUT : INPUT);
            pDigitalWrite(pins[i],LOW);
        }
    }
    pDigitalWrite(pins[ledRow],HIGH);
    pPinmode(pins[ledRow],OUTPUT);
    if (++ledRow >= numberOfPins)
    {
        ledRow = 0;
    }
}
