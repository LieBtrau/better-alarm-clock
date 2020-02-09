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
// Attach a rotary encoder with output pins to A2 and A3.
// The common contact should be attached to ground.

#include <RotaryEncoder.h>

// Setup a RoraryEncoder for pins A2 and A3:
RotaryEncoder encoder(A2, A3);

void blink()
{
  encoder.tick(); // just call tick() to check the state.
}

void setup()
{
  Serial.begin(115200);
  Serial.println("SimplePollRotator example for the RotaryEncoder library.");

  pinMode(A2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(A2), blink, CHANGE);
  pinMode(A3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(A3), blink, CHANGE);
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
} // loop ()

// The End
