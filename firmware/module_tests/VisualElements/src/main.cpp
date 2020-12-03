#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include "SPI.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "Visuals.h"

Adafruit_7segment matrix = Adafruit_7segment();
SevenSegmentField alarmHoursDisplay(&matrix);
SevenSegmentField alarmMinutesDisplay(&matrix);
byte hours = 0;
byte minutes = 0;

void setup()
{
  // put your setup code here, to run once:
#ifndef __AVR_ATtiny85__
  Serial.begin(115200);
  Serial.printf("Build %s\r\n", __TIMESTAMP__);
#endif
  if (!alarmHoursDisplay.init(0) || !alarmMinutesDisplay.init(3))
  {
    Serial.println("Can't init alarm display.");
    while (true)
      ;
  }
  alarmHoursDisplay.setVisible(true);
  alarmMinutesDisplay.setVisible(true);
}

void loop()
{
  alarmHoursDisplay.flash();
  // put your main code here, to run repeatedly:
  alarmHoursDisplay.setValue(hours);
  alarmMinutesDisplay.setValue(minutes);
  // delay(100);
  // alarmMinutesDisplay.hide();
  // delay(100);
  alarmMinutesDisplay.setBrightness(minutes & 0xF);
  if (++minutes > 59)
  {
    hours = hours == 23 ? 0 : hours + 1;
    minutes = 0;
  }
  if(alarmMinutesDisplay.render() | alarmHoursDisplay.render()) //don't use double |, because both statements need to be executed.
  {
        matrix.writeDisplay();
  }
  delay(100);
}