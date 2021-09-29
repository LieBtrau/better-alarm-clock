#pragma once
#include "Arduino.h"

//DCF
const byte pin_DCF = PB4;
const byte pin_nEn_DCF = PB1;

//GPIO
const byte pin_KeyPad_IRQ = PB11;

//PIR
const byte pin_PIR = PB5;

//Rotary Encoder
const byte pin_Clk = PB12;
const byte pin_Data = PB13;
const byte pin_switch = PA0;

//I²C
const byte pinSCL = PB6;
const byte pinSDA = PB7;

//SPI
const byte pinMOSI = PA7;
const byte pinSCLK = PA5;
const byte pinCS = PA4;

//MP3
const byte pinPlayBusy = PA1;
const byte pin_TX2 = PA2;
const byte pin_RX2 = PA3;

//Sunrise simulation
const byte pin_pwmh = PB8;
const byte pin_pwml = PB9;
const byte pin_en_sun = PA8;

//SX1509 LED pins
const byte pin_sunlight = 7;
const byte pin_volume = 6;
const byte pin_songChoice = 5;
const byte pin_alarm = 4;
const byte pin_Monday = 4;
const byte pin_Tuesday = 5;
const byte pin_Wednesday = 6;
const byte pin_Thursday = 12;
const byte pin_Friday = 13;
const byte pin_Saturday = 14;
const byte pin_Sunday = 15;

