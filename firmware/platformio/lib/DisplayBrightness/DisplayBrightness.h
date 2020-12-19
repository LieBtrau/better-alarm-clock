#pragma once

#include "AsyncDelay.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include "PirSensor.h"

class DisplayBrightness
{
public:
    DisplayBrightness(PirSensor *ps, Adafruit_TSL2591 *tsl);
    bool init();
    bool isDisplayOn();
    byte getDisplayBrightness();

private:
    const int LIGHT_SENSOR_READ_INTERVAL = 10000;
    const int DISPLAY_DARK_TIMEOUT = 15000;
    bool movementDetected();
    byte _displayBrightness = 0xFF;
    byte _desiredDisplayBrightness = 5;
    PirSensor *_ps;
    Adafruit_TSL2591 *_tsl;
    AsyncDelay _ambientLightSenseTimer; //Control how often the light sensor is read
    AsyncDelay _displayOnTimer;         //Control how long the display should be on
    int _lastBrightness = 0;
};