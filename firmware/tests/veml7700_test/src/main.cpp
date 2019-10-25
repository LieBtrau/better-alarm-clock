/*!
  * file readVEML7700.ino
  * brief DFRobot's VEML7700 ambient light sensor
  * @n [Get the module here](等上架后添加商品购买链接)
  * @n This example Set the volume size and play music snippet.
  * @n [Connection and Diagram](等上架后添加wiki链接)
  *
  * Copyright	[DFRobot](http://www.dfrobot.com), 2016
  * Copyright	GNU Lesser General Public License
  *
  * @author [yangyang](971326313@qq.com)
  * version  V1.0
  * date  2016-12-8
  */
#define SOFTWIRE 1
#ifdef SOFTWIRE
#include "SoftWire.h"
#ifdef ARDUINO_ARCH_AVR
uint8_t sdaPin = A4;
uint8_t sclPin = A5;
#warning hello using softwire
#else
// Adjust to suit your non-AVR architecture
uint8_t sdaPin = 0;
uint8_t sclPin = 1;
#endif
SoftWire i2c(sdaPin, sclPin);
#else
#include "Wire.h"
#endif
#include "DFRobot_VEML7700.h"

DFRobot_VEML7700 als;

void setup()
{
  Serial.begin(9600);
  Serial.print("started");
#ifdef SOFTWIRE
  i2c.setDelay_us(5);
  i2c.begin();
  i2c.setTxBuffer(malloc(50), 50);
  i2c.setRxBuffer(malloc(10),10);
  if (!als.begin(&i2c))
#else
  if (!als.begin(&Wire))
#endif
  {
    while (true)
      ;
  }
}

void loop()
{
  /*
  float lux;
  als.getALSLux(lux);
  Serial.print("Lux:");
  Serial.print(lux);
  */
  uint16_t alsval;
  uint16_t whiteVal;
  als.getALS(alsval);
  als.getWhite(whiteVal);
  Serial.print(alsval);
  Serial.print("\t");
  Serial.println(whiteVal);
  delay(2000);
}
