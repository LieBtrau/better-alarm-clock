#pragma once

#include "millisDelay.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"

class DisplayBrightness
{
    public:
    DisplayBrightness(byte pir_pin);
    bool init();
    bool getDisplayBrightness(byte& brightness);
    bool isDisplayOn(bool buttonPressed);
private:
    const int DISPLAY_DARK_TIMEOUT = 15000;
    bool movementDetected();
    byte _displayBrightness=0xFF;
    byte _desiredDisplayBrightness=5;
    millisDelay _clockRefreshTimer;
    millisDelay _displayOffTimer;
    millisDelay _ambientLightSenseTimer;
    Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);
    byte _pir_pin;
    int _lastBrightness=0;
};