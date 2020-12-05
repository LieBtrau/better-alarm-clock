#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include "SPI.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "Visuals.h"
#include "Max72xxPanel.h"
#include <SparkFunSX1509.h>
#include "pins.h"

int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 2;

Max72xxPanel matrix = Max72xxPanel(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
Adafruit_7segment sevenSegment = Adafruit_7segment();
const byte IO1_SX1509_ADDRESS = 0x3E; // SX1509 I2C address
const byte IO2_SX1509_ADDRESS = 0x3F; // SX1509 I2C address
SX1509 io1, io2;                      // Create an SX1509 object to be used throughout

SevenSegmentField alarmHoursDisplay(&sevenSegment,0);
SevenSegmentField alarmMinutesDisplay(&sevenSegment,3);
ClockFace cf(&matrix);
LedMatrixField lmf(&matrix, {0, 0}, {11, 2}, 10);
LedMatrixSelect lms(&matrix, {0, 7}, {11, 9}, 10);
LedToggle lt14(&io1, 4);
LedToggle lt25(&io2, 5);

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
  if (!io1.begin(IO1_SX1509_ADDRESS))
  {
    Serial.println("Failed to communicate with io1");
    while (1)
      ; // If we fail to communicate, loop forever.
  }
  if (!io2.begin(IO2_SX1509_ADDRESS))
  {
    Serial.println("Failed to communicate with io2");
    while (1)
      ; // If we fail to communicate, loop forever.
  }
  io1.clock(INTERNAL_CLOCK_2MHZ, 3);
  io2.clock(INTERNAL_CLOCK_2MHZ, 3);
  lt14.init();
  lt25.init();

  alarmHoursDisplay.setVisible(true);
  alarmMinutesDisplay.setVisible(true);
  cf.setVisible(false);
  lmf.setVisible(true);
  lms.setVisible(true);
  lt14.setVisible(true);
  lt25.setVisible(true);
}

void loop()
{
  alarmHoursDisplay.flash();
  // put your main code here, to run repeatedly:
  alarmHoursDisplay.setValue(hours);
  alarmMinutesDisplay.setValue(minutes);
  //cf.setTime(hours, minutes, true);
  lmf.setValue(minutes / 6);
  lms.setValue(minutes / 6);

  sevenSegment.setBrightness(minutes & 0xF);
  matrix.setIntensity(minutes & 0xF);
  lt14.setBrightness(minutes << 2);
  lt25.setBrightness(minutes << 2);

  if (++minutes > 59)
  {
    hours = hours == 23 ? 0 : hours + 1;
    minutes = 0;
  }

  if (alarmMinutesDisplay.render() | alarmHoursDisplay.render()) //don't use double |, because both statements need to be executed.
  {
    sevenSegment.writeDisplay();
  }
  if (cf.render() | lmf.render() | lms.render())
  {
    matrix.write();
  }
  lt14.render();
  lt25.render();
  delay(100);
}