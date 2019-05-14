#include "Max72xxPanel.h"
#include <SPI.h>
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix7 = Adafruit_7segment();
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 2;
const byte pinMOSI = PA7;
const byte pinSCLK = PA5;
const byte pinCS = PA1;

Max72xxPanel matrix = Max72xxPanel(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

void setup()
{
  matrix.init();
  matrix.setCursor(2, 5);
  matrix.print("99:99");
  matrix.write(); // Send bitmap to display

#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  Serial.println("7 Segment Backpack Test");
#endif
  matrix7.begin(0x70);
}

void loop() {
  // try to print a number thats too long
  matrix7.print(10000, DEC);
  matrix7.writeDisplay();
  delay(500);

  // print a hex number
  matrix7.print(0xBEEF, HEX);
  matrix7.writeDisplay();
  delay(500);

  // print a floating point 
  matrix7.print(12.34);
  matrix7.writeDisplay();
  delay(500);
  
  // print with print/println
  for (uint16_t counter = 0; counter < 9999; counter++) {
    matrix7.println(counter);
    matrix7.writeDisplay();
    matrix.fillScreen(0);
    matrix.setCursor(2, 5);
    matrix.print(counter);
    matrix.write();
    delay(10);
  }

  // method #2 - draw each digit
  uint16_t blinkcounter = 0;
  boolean drawDots = false;
  for (uint16_t counter = 0; counter < 9999; counter ++) {
    matrix7.writeDigitNum(0, (counter / 1000), drawDots);
    matrix7.writeDigitNum(1, (counter / 100) % 10, drawDots);
    matrix7.drawColon(drawDots);
    matrix7.writeDigitNum(3, (counter / 10) % 10, drawDots);
    matrix7.writeDigitNum(4, counter % 10, drawDots);
   
    blinkcounter+=50;
    if (blinkcounter < 500) {
      drawDots = false;
    } else if (blinkcounter < 1000) {
      drawDots = true;
    } else {
      blinkcounter = 0;
    }
    matrix7.writeDisplay();
    delay(10);
  }
}