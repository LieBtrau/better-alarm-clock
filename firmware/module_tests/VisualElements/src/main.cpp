#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include "SPI.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "Visuals.h"
#include "Max72xxPanel.h"
#include "pins.h"

int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 2;

Max72xxPanel matrix = Max72xxPanel(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
Adafruit_7segment sevenSegment = Adafruit_7segment();

SevenSegmentField alarmHoursDisplay(&sevenSegment);
SevenSegmentField alarmMinutesDisplay(&sevenSegment);
ClockFace cf(&matrix);

byte hours = 0;
byte minutes = 0;

void setup()
{
  // put your setup code here, to run once:
#ifndef __AVR_ATtiny85__
  Serial.begin(115200);
  Serial.printf("Build %s\r\n", __TIMESTAMP__);
#endif
  matrix.init();
  sevenSegment.begin(0x70);

  if (!alarmHoursDisplay.init(0) || !alarmMinutesDisplay.init(3))
  {
    Serial.println("Can't init alarm display.");
    while (true)
      ;
  }
  alarmHoursDisplay.setVisible(true);
  alarmMinutesDisplay.setVisible(true);
  cf.setVisible(true);
}

void loop()
{
  alarmHoursDisplay.flash();
  // put your main code here, to run repeatedly:
  alarmHoursDisplay.setValue(hours);
  alarmMinutesDisplay.setValue(minutes);
  cf.setTime(hours, minutes, true);

  sevenSegment.setBrightness(minutes & 0xF);
  matrix.setIntensity(minutes & 0xF);

  if (++minutes > 59)
  {
    hours = hours == 23 ? 0 : hours + 1;
    minutes = 0;
  }

  if (alarmMinutesDisplay.render() | alarmHoursDisplay.render()) //don't use double |, because both statements need to be executed.
  {
    sevenSegment.writeDisplay();
  }
  if(cf.render())
  {
    matrix.write();
  }
  delay(100);
}