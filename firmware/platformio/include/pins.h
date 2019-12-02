#pragma once
#include "Arduino.h"

//I²C
const byte pinSCL = PB6;
const byte pinSDA = PB7;

//SPI
const byte pinMOSI = PA7;
const byte pinSCLK = PA5;
const byte pinCS = PA1;

//GPIO
const byte pinIRQ = PB1;
const byte pinPlayBusy = PA4;
const byte pin_DCF = PA8;
const byte pin_TRG = PB15;
const byte pin_PIR = PB5;

//Serial2
const byte pin_TX2 = PA2;
const byte pin_RX2 = PA3;