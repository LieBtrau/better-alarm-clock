/******************************************************************
 A library for controling a set of 8x8 LEDs with a MAX7219 or
 MAX7221 displays.
 This is a plugin for Adafruit's core graphics library, providing
 basic graphics primitives (points, lines, circles, etc.).
 You need to download and install Adafruit_GFX to use this library.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source hardware
 by purchasing products from Adafruit!

 Written by Mark Ruys.
 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.
 ******************************************************************/

#include <Adafruit_GFX.h>
#include "Max72xxPanel.h"
#include <SPI.h>

// The opcodes for the MAX7221 and MAX7219
#define OP_NOOP 0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE 9
#define OP_INTENSITY 10
#define OP_SCANLIMIT 11
#define OP_SHUTDOWN 12
#define OP_DISPLAYTEST 15

Max72xxPanel::Max72xxPanel(byte dataPin, byte clkPin, byte csPin, byte hDisplays, byte vDisplays) : Adafruit_GFX(hDisplays << 3, vDisplays << 3)
{
	SPI_CS = csPin;
	SPI_MOSI = dataPin;
	SPI_CLK = clkPin;
	pinMode(SPI_CS, OUTPUT);
	pinMode(SPI_MOSI, OUTPUT);
	pinMode(SPI_CLK, OUTPUT);

	byte displays = hDisplays * vDisplays;
	bitmapSize = displays << 3;
	bitmap = (byte *)malloc(bitmapSize);
}

Max72xxPanel::Max72xxPanel(byte csPin, byte hDisplays, byte vDisplays) : Max72xxPanel(0, 0, csPin, hDisplays, vDisplays)
{
	useSoftSpi = false;
}

void Max72xxPanel::init()
{
	pinMode(SPI_CS, OUTPUT);
	digitalWrite(SPI_CS, HIGH);
	if (SPI_MOSI > 0)
	{
		pinMode(SPI_MOSI, OUTPUT);
	}
	if (SPI_CLK > 0)
	{
		pinMode(SPI_CLK, OUTPUT);
	}
	for (int i = 0; i < 2; i++)
	{
		// Make sure we are not in test mode
		spiTransfer(OP_DISPLAYTEST, 0);

		// We need the multiplexer to scan all segments
		spiTransfer(OP_SCANLIMIT, 7);

		// We don't want the multiplexer to decode segments for us
		spiTransfer(OP_DECODEMODE, 0);

		// Enable display
		shutdown(false);

		// Set the brightness to a medium value
		setIntensity(7);

		// Clear the screen
		fillScreen(0);
		write(true);
		delay(100);
	}
}

void Max72xxPanel::shutdown(bool b)
{
	spiTransfer(OP_SHUTDOWN, b ? 0 : 1);
}

void Max72xxPanel::setIntensity(byte intensity)
{
	spiTransfer(OP_INTENSITY, intensity);
}

void Max72xxPanel::fillScreen(uint16_t color)
{
	memset(bitmap, color ? 0xff : 0, bitmapSize);
}

void Max72xxPanel::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	byte dispY = y >> 3; // divide by # rows per display
	byte dispX = x >> 3; // divide by # rows per display | each display has 8 rows, so display X-offset is a multiple of 8.
	byte *ptr = &bitmap[WIDTH * dispY + (dispX << 3) + (y & 0b111)];
	byte val = 1 << ((7 - x) & 0b111);

	if (color)
	{
		*ptr |= val;
	}
	else
	{
		*ptr &= ~val;
	}
	refreshNeeded = true;
}

void Max72xxPanel::write(bool forceRefresh)
{
	// Send the bitmap buffer to the displays.
	if (!forceRefresh && !refreshNeeded)
	{
		return;
	}
	for (byte row = OP_DIGIT7; row >= OP_DIGIT0; row--)
	{
		spiTransfer(row, 0);
	}
	refreshNeeded = false;
}

void Max72xxPanel::spiTransfer(byte opcode, byte data)
{
	if (useSoftSpi)
	{
		softSpiTransfer(opcode, data);
	}
	else
	{
		hardSpiTransfer(opcode, data);
	}
}
void Max72xxPanel::softSpiTransfer(byte opcode, byte data)
{
	// If opcode > OP_DIGIT7, send the opcode and data to all displays.
	// If opcode <= OP_DIGIT7, display the column with data in our buffer for all displays.
	// We do not support (nor need) to use the OP_NOOP opcode.

	// Enable the line
	digitalWrite(SPI_CS, LOW);

	// Now shift out the data, two bytes per display. The first byte is the opcode,
	// the second byte the data.
	byte end = opcode - OP_DIGIT0;
	byte start = bitmapSize + end;
	do
	{
		start -= 8;
		shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, opcode);
		shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, opcode <= OP_DIGIT7 ? bitmap[start] : data);
	} while (start > end);

	// Latch the data onto the display(s)
	digitalWrite(SPI_CS, HIGH);
}

void Max72xxPanel::hardSpiTransfer(byte opcode, byte data)
{
	// If opcode > OP_DIGIT7, send the opcode and data to all displays.
	// If opcode <= OP_DIGIT7, display the column with data in our buffer for all displays.
	// We do not support (nor need) to use the OP_NOOP opcode.

	SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
	// Enable the line
	digitalWrite(SPI_CS, LOW);

	// Now shift out the data, two bytes per display. The first byte is the opcode,
	// the second byte the data.
	byte end = opcode - OP_DIGIT0;
	byte start = bitmapSize + end;
	do
	{
		start -= 8;
		SPI.transfer(opcode);
		SPI.transfer(opcode <= OP_DIGIT7 ? bitmap[start] : data);
	} while (start > end);

	// Latch the data onto the display(s)
	digitalWrite(SPI_CS, HIGH);
	SPI.endTransaction();
}
