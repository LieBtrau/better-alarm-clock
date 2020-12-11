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
  // alarmHoursDisplay.setFlashMode();
  // // put your main code here, to run repeatedly:
  // alarmHoursDisplay.setValue(hours);
  // alarmMinutesDisplay.setValue(minutes);

  // lmf_SunLightBrightness.setValue(minutes / 6);
  // lms_SongChoice.setValue(minutes / 6);
  showAlarmDisplay((ALARM_DISPLAY)(minutes & 0x3));

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