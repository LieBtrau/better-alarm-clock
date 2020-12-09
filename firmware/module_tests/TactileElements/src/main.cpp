#include <Arduino.h>
#include "inputs.h"

AsyncDelay ledTimer(500, AsyncDelay::MILLIS);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial)
    ;
  delay(200);
  Serial.printf("Build %s\r\n", __TIMESTAMP__);
  if (!setupInputs())
  {
    Serial.println("Can't setup all input devices.");
    while (true)
      ;
  }
}

void loop()
{
  RotaryEncoder_Tactiles::DIRECTION dir;
  KEY_CODE key;
  if (isRotaryEncoderChanged(dir))
  {
    Serial.println(dir, DEC);
  }
  if(isButtonChanged(key))
  {
    Serial.println(key);
  }
  if(ledTimer.isExpired())
  {
    ledTimer.restart();
    digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ? LOW : HIGH);
  }
}