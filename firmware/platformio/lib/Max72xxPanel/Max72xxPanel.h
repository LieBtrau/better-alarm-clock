/******************************************************************
 A library for controling a set of 8x8 LEDs with a MAX7219 or
 MAX7221 displays.

 This is a plugin for Adafruit's core graphics library, providing
 basic graphics primitives (points, lines, circles, etc.).
 You need to download and install Adafruit_GFX to use this library.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source hardware
 by purchasing products from Adafruit!

 Written by Mark Ruys, 2013.
 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.

 Datasheet: http://datasheets.maximintegrated.com/en/ds/MAX7219-MAX7221.pdf

 ******************************************************************/

#pragma once

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#include "pins_arduino.h"
#endif
#include <Adafruit_GFX.h>

class Max72xxPanel : public Adafruit_GFX
{

public:
  /*
   * Create a new controler
   * Parameters:
   * csPin		pin for selecting the device
   * hDisplays  number of displays horizontally
   * vDisplays  number of displays vertically
   */
  Max72xxPanel(byte csPin, byte hDisplays = 1, byte vDisplays = 1);
  Max72xxPanel(byte dataPin, byte clkPin, byte csPin, byte hDisplays, byte vDisplays = 1);
  void init();

  /*
   * Draw a pixel on your canvas. Note that for performance reasons,
   * the pixels are not actually send to the displays. Only the internal
   * bitmap buffer is modified.
   */
  void drawPixel(int16_t x, int16_t y, uint16_t color);

  /*
   * As we can do this much faster then setting all the pixels one by
   * one, we have a dedicated function to clear the screen.
   * The color can be 0 (blank) or non-zero (pixel on).
   */
  void fillScreen(uint16_t color);

  /*
   * Set the shutdown (power saving) mode for the device
   * Paramaters:
   * status	If true the device goes into power-down mode. Set to false
   *		for normal operation.
   */
  void shutdown(bool status);

  /*
   * Set the brightness of the display.
   * Paramaters:
   * intensity	the brightness of the display. (0..15)
   */
  void setIntensity(byte intensity);

  /*
   * After you're done filling the bitmap buffer with your picture,
   * send it to the display(s).
   */
  void write(bool forceRefresh=false);

private:
  byte SPI_CS; /* SPI chip selection */
  byte SPI_MOSI;
  byte SPI_CLK;

  /* Send out a single command to the device */
  void hardSpiTransfer(byte opcode, byte data);
  void softSpiTransfer(byte opcode, byte data);
  void spiTransfer(byte opcode, byte data);

  /* We keep track of the led-status for 8 devices in this array */
  byte *bitmap;
  byte bitmapSize;
  bool useSoftSpi = true;
  bool refreshNeeded = true;
};
