// -----
// InterruptRotator.ino - Example for the RotaryEncoder library.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
// -----
// 18.01.2014 created by Matthias Hertel
// -----

// This example checks the state of the rotary encoder in the loop() function.
// The current position is printed on output when changed.

// Hardware setup:
// Attach a rotary encoder with output pins to PB12 and PB13.
// The common contact should be attached to ground.

#include <RotaryEncoder.h>

// Setup a RoraryEncoder for the following pins:
const int pinClk = PB12;
const int pinData = PB13;
const int pinSwitch = PA0;
RotaryEncoder encoder(pinData, pinClk);

void blink()
{
  encoder.tick(); // just call tick() to check the state.
}

void switchAction()
{
  Serial.printf("Switch: %s\r\n",digitalRead(pinSwitch)? "pressed": "released");
}

void setup()
{
  Serial.begin(115200);
  while(!Serial);
  delay(100);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.printf("Build %s\r\n", __TIMESTAMP__);

  pinMode(pinClk, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinClk), blink, CHANGE);
  pinMode(pinData, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinData), blink, CHANGE);
  pinMode(pinSwitch, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pinSwitch), switchAction, CHANGE);

} // setup()

// Read the current position of the encoder and print out when changed.
void loop()
{
  static int pos = 0;

  int newPos = encoder.getPosition();
  if (pos != newPos)
  {
    Serial.print(newPos);
    Serial.println();
    pos = newPos;

   // Just to show, that long lasting procedures don't break the rotary encoder:
    // When newPos is 66 the ouput will freeze, but the turned positions will be recognized even when not polled.
    // The interrupt still works.
    // The output is correct 6.6 seconds later.
    if (newPos == 66)
      delay(6600);
  } // if
    digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ? 0 : 1);
    delay(200);
 } // loop ()

// The End
