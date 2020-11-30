#include <Arduino.h>
#include "pins.h"
#include "LedDriverDimming.h"

byte lightValue = 0;
bool directionUp = true;
LedDriverDimming ldd(pin_en_sun, pin_pwmh, pin_pwml);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    ;
  delay(100);
  Serial.printf("Build %s\r\n", __TIMESTAMP__);
  ldd.init();
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(100);
  Serial.printf("lightValue: %d\r\n", lightValue);
  if (directionUp)
  {
    if (lightValue < 100)
    {
      lightValue+=1;
    }
    else
    {
      directionUp = false;
    }
  }
  else
  {
    if (lightValue > 0)
    {
      lightValue-=1;
    }
    else
    {
      directionUp = true;
    }
  }
  ldd.setBrightness(lightValue);
}