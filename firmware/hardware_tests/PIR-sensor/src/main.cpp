//This doesn't really involve software, but it's easy.  No DMM needed.

#include <Arduino.h>

const byte PIRpin = PB5;

void setup()
{
  // put your setup code here, to run once:
  pinMode(PIRpin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  //Detection = LED ON
  digitalWrite(LED_BUILTIN, digitalRead(PIRpin) ? 0 : 1);
}