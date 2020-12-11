#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include "SPI.h"
#include "VisualElements.h"

byte hours = 0;
byte minutes = 0;

void setup()
{
  Serial.begin(115200);
  Serial.printf("Build %s\r\n", __TIMESTAMP__);
  if (!initVisualElements())
  {
    Serial.println("Failed to initialize visual elements.");
    while (1)
      ; // If we fail to communicate, loop forever.
  }
}

void loop()
{
  showAlarmDisplay((ALARM_DISPLAY)(minutes & 0x3));
  showAlarmTime(hours, minutes);
  setBrightness(minutes & 0xF);
  //sre.setBrightness(minutes * 100 / 60);
  showWeekDay((WEEKDAYS)minutes);

  if (millis() > 5000)
  {
    if (++minutes > 59)
    {
      hours = hours == 23 ? 0 : hours + 1;
      minutes = 0;
    }
    showTime(hours, minutes, true);
  }
  redraw();
  delay(100);
}