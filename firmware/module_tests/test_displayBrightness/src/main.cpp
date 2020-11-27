/* 
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include "PirSensor.h"
#include "DisplayBrightness.h"

PirSensor ps(PB5);
Adafruit_TSL2591 tsl(2591);
DisplayBrightness db(&ps, &tsl);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    ;
  delay(100);
  Serial.printf("Build %s\r\n", __TIMESTAMP__);
  if (!db.init())
  {
    while (true)
      ;
  }
  Serial.println("All initialized");
}

void loop()
{
  // put your main code here, to run repeatedly:
  byte brightness;
  if (db.getDisplayBrightness(brightness))
  {
    Serial.printf("New brightness: %d\r\n", brightness);
  }
  Serial.printf("Display: %s\r\n", db.isDisplayOn(false) ? "on" : "off");
  delay(1000);
}