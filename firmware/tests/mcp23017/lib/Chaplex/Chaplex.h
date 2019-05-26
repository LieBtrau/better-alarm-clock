/*
||
|| @author Stefan Götze 
||         using Alexander Brevig - Charlieplex library
|| @version 0.1
||
|| Updates by Christoph Tack
||
*/

#ifndef Chaplex_h
#define Chaplex_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

typedef enum
{
    OFF=0,
    ON=1
}LEDSTATE;

struct CharlieLed {
  byte a;
  byte c; 
} ;

typedef struct CharlieLed charlieLed;
class Chaplex {

public:
    Chaplex();
    bool setLedState(charlieLed led, LEDSTATE state);
	void allClear();
    bool showLedState(byte &pinModes, byte &gpioStates);
    void setSingleLed(charlieLed led, LEDSTATE state);
private:
    byte ledCtrl[5];
    byte ledRow=0;
};

#endif
