/***************************************************************************
  This is a library for the APDS9960 digital proximity, ambient light, RGB, and gesture sensor

  This sketch puts the sensor in color mode and reads the RGB and clear values.

  Designed specifically to work with the Adafruit APDS9960 breakout
  ----> http://www.adafruit.com/products/3595

  These sensors use I2C to communicate. The device's I2C address is 0x39

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include "Adafruit_APDS9960.h"
Adafruit_APDS9960 apds;

void setup() {
  Serial.begin(115200);

  if(!apds.begin()){
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else Serial.println("Device initialized!");

  //enable color sensign mode
  apds.enableColor(true);
}

void loop() {
  //create some variables to store the color data in
  uint16_t r, g, b, c;
  
  //wait for color data to be ready
  while(!apds.colorDataReady()){
    delay(5);
  }

  //get the data and print the different channels
  apds.getColorData(&r, &g, &b, &c);
  
  Serial.print(" clear: ");
  Serial.println(c);
  Serial.println();
  
  delay(500);
}


/**
 * https://www.maximintegrated.com/en/app-notes/index.mvp/id/4913
 * f(x) = Brightness ratio [0 100]
 * x = ambient light [lux]
 * x<1254 -> f(x) = 9.9323*ln(x)+27.059
 * x>=1254 -> f(x) = 100
 * The resolution for the APDS-9960 is 1 lux, which is the smallest value that can be measured.  This corresponds to a brightness setting of 27%.
 * So the bottom 27% of our range would be wasted.
 * Display has only 16 brightness settings [0 15] -> [1 16] in the calculation, so better calculate lux values from brightness settings
 *  x = e ^ ((f(x) - 27.059)/9.9323)