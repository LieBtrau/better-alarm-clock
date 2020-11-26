//This doesn't really involve software, but it's easy.  No DMM needed.

#include <Arduino.h>
#include "PirSensor.h"
const byte PIRpin = PB5;

PirSensor ps(PIRpin);
void setup()
{
  // put your setup code here, to run once:
  ps.init();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  //On Blue Pill, builtin LED is active low.
  digitalWrite(LED_BUILTIN, ps.movementDetected() ? LOW : HIGH);
}