#pragma once
#include "Arduino.h"

//DCF
const byte pinDCF = PA8;
const byte pin_nEn_DCF = PB1;

//GPIO
const byte pin_KeyPad_IRQ = PB11;
const byte pinPlayBusy = PA4;
const byte pin_DCF = PA8;
const byte pin_TRG = PB15;
const byte pin_PIR = PB5;

//Rotary Encoder
const byte pin_Clk = PB12;
const byte pin_Data = PB13;


//I²C
const byte pinSCL = PB6;
const byte pinSDA = PB7;

//SPI
const byte pinMOSI = PA7;
const byte pinSCLK = PA5;
const byte pinCS = PA4;

//Serial2
const byte pin_TX2 = PA2;
const byte pin_RX2 = PA3;

//Sunrise simulation
const byte pin_pwmh = PB8;
const byte pin_pwml = PB9;
const byte pin_en_sun = PB4;