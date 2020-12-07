#include <Arduino.h>
#include "Tactiles.h"
#include "pins.h"


//Hardware peripherals
SX1509 io1, io2;

//Tactile elements
RotaryEncoder_Tactiles ret(pin_Data, pin_Clk);
Sx1509_Tactiles st(&io1, pin_KeyPad_IRQ);

void setup()
{
  const byte IO1_SX1509_ADDRESS = 0x3E; // SX1509 I2C address
  const byte IO2_SX1509_ADDRESS = 0x3F; // SX1509 I2C address

  // put your setup code here, to run once:
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
  Serial.printf("Build %s\r\n", __TIMESTAMP__);

  // Use the internal 2MHz oscillator.
  // Set LED clock to 500kHz (2MHz / (2^(3-1)):
  io1.clock(INTERNAL_CLOCK_2MHZ, 3);
  io2.clock(INTERNAL_CLOCK_2MHZ, 3);

  st.init(4,2);

  Serial.begin(115200);
  while (!Serial)
    ;
  delay(100);
  Serial.printf("Build %s\r\n", __TIMESTAMP__);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (ret.isChanged())
  {
    Serial.println(ret.getValue());
  }
  if(st.isChanged())
  {
    Serial.println(st.getValue());
  }
}